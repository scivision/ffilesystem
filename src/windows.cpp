#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winioctl.h>
#endif

#include <cstddef>  // for std::byte
#include <cstdlib>  // for _splitpath_s, _MAX_*
#include <iostream>

#include <string>
#include <string_view>

#include <system_error>

#include "ffilesystem.h"
#include "win32_path.h"


#if defined(_WIN32)
std::optional<fs_win32_path_parts>
fs_win32_split_path(std::string_view path)
{
  fs_win32_path_parts parts{
    std::string(_MAX_DRIVE, '\0'),
    std::string(_MAX_DIR, '\0')
  };
  const std::string cpath{path};

  if (_splitpath_s(cpath.c_str(), parts.drive.data(), parts.drive.size(),
                   parts.directory.data(), parts.directory.size(),
                   nullptr, 0, nullptr, 0) != 0)
    return std::nullopt;

  ffilesystem::trim(parts.drive);
  ffilesystem::trim(parts.directory);
  return parts;
}
#endif


namespace ffilesystem {

bool is_appexec_alias(std::string_view path)
{
// Windows App Execution Alias allow easy access to Windows Store apps
// from the Windows Terminal. However, they don't work with _access_s()
// or stat() like regular files. This function detects the aliases.
// Reference:
// https://learn.microsoft.com/en-us/windows/terminal/command-line-arguments?tabs=windows#add-windows-terminal-executable-to-your-path
// https://learn.microsoft.com/en-us/windows/win32/fileio/reparse-points
//
// https://learn.microsoft.com/en-us/windows/win32/fileio/reparse-point-tags

#if defined(_WIN32) || defined(__CYGWIN__)
  HANDLE h = fs_win32_get_file_handle(path, FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS);

  if (h == INVALID_HANDLE_VALUE) {
    const DWORD err = GetLastError();
    if (err != ERROR_FILE_NOT_FOUND)
      fs_error_callback(path, "CreateFileW failed");
    return false;
  }

  FILE_ATTRIBUTE_TAG_INFO t;
  // https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getfileinformationbyhandleex
  BOOL ok = GetFileInformationByHandleEx(h, FileAttributeTagInfo, &t, sizeof(t));
  CloseHandle(h);
  if (ok) {
    return t.FileAttributes & FILE_ATTRIBUTE_REPARSE_POINT &&
           t.ReparseTag == IO_REPARSE_TAG_APPEXECLINK;
  }
  fs_error_callback(path, "GetFileInformationByHandleEx failed");
  return false;

#else
  fs_error_callback(path, std::make_error_code(std::errc::function_not_supported));
  return false;
#endif

}

} // namespace ffilesystem


#if defined(_WIN32) || defined(__CYGWIN__)
HANDLE fs_win32_get_file_handle(std::string_view path, const DWORD flags)
{
  // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew
  // FILE_FLAG_BACKUP_SEMANTICS is required to open a handle to a directory, else ERROR_ACCESS_DENIED
  return CreateFileW(fs_win32_to_wide(path).c_str(),
                     0,
                     FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                     nullptr,
                     OPEN_EXISTING,
                     flags,
                     nullptr);
}
#endif


bool fs_win32_long_paths_enabled() {
  // from https://github.com/microsoft/STL/pull/5783/
  // microsoft/STL has Apache 2.0 license
  // https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation?tabs=powershell
  // https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-reggetvaluew

#if defined(_WIN32)
  DWORD registry_value = 0;
  DWORD buffer_size    = sizeof(registry_value);
  const LSTATUS status = RegGetValueW(HKEY_LOCAL_MACHINE, LR"(SYSTEM\CurrentControlSet\Control\FileSystem)",
      L"LongPathsEnabled", RRF_RT_REG_DWORD, nullptr, &registry_value, &buffer_size);

  switch (status) {
  case ERROR_SUCCESS:
      if(buffer_size == sizeof(registry_value))
        return registry_value != 0;
      break;
  case ERROR_FILE_NOT_FOUND:
      return false; // The registry value doesn't exist, so long paths aren't enabled.
  case ERROR_MORE_DATA:
      std::cerr << "fs_win32_long_paths_enabled: RegGetValueW() returned ERROR_MORE_DATA; this should not be possible.\n";
      break;
  case ERROR_UNSUPPORTED_TYPE:
      std::cerr << "fs_win32_long_paths_enabled: RegGetValueW() returned ERROR_UNSUPPORTED_TYPE; the value exists but has a weird type.\n";
      break;
  default:
      std::cerr << "fs_win32_long_paths_enabled: RegGetValueW() returned " << status << ".\n";
      break;
  }

#else
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
#endif

  return false;
}


#if defined(_WIN32) || defined(__CYGWIN__)
bool fs_win32_is_symlink(std::string_view path)
{
// distinguish between Windows symbolic links and reparse points as
// reparse points can be unlike symlinks.
// IO_REPARSE_TAG_MOUNT_POINT Junction is a type of reparse point
// that we treat as a symlink.
//
// Using 0 instead of GENERIC_READ as it allows reading of file attributes
// even if we do not have permission to read the file itself

  HANDLE h = fs_win32_get_file_handle(path, FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS);

  if (h == INVALID_HANDLE_VALUE) {
    const DWORD err = GetLastError();
    if (err != ERROR_FILE_NOT_FOUND && err != ERROR_PATH_NOT_FOUND)
      fs_error_callback(path, "CreateFileW failed");
    return false;
  }

  FILE_ATTRIBUTE_TAG_INFO t;
  BOOL ok = GetFileInformationByHandleEx(h, FileAttributeTagInfo, &t, sizeof(t));
  CloseHandle(h);
  if (ok) {
    return t.FileAttributes & FILE_ATTRIBUTE_REPARSE_POINT &&
      (t.ReparseTag == IO_REPARSE_TAG_SYMLINK || t.ReparseTag == IO_REPARSE_TAG_MOUNT_POINT);
  }
  fs_error_callback(path, "GetFileInformationByHandleEx failed");
  return false;
}
#endif


bool
fs_win32_is_ext_path(std::string_view path)
{
  // Windows extended path prefix enables paths longer than MAX_PATH (260 characters)
  // and disables path parsing, so that special characters are allowed

  //   \\?\ - enables extended-length paths and disables path parsing
constexpr std::string_view ext = R"(\\?\)";
  //   \\.\ - used for device paths (e.g., physical drives, COM ports)
constexpr std::string_view dev = R"(\\.\)";

  #ifdef __cpp_lib_starts_ends_with  // C++20
    return path.starts_with(ext) || path.starts_with(dev);
  #else
    if (path.length() < 4)
      return false;

    std::string_view p = path.substr(0, 4);
    return p == ext || p == dev;
  #endif
}



#if defined(_WIN32) || defined(__CYGWIN__)
std::string
fs_win32_full_name(std::string_view path)
{
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfullpathnameW
// GetFinalPathNameByHandle or GetFullPathName returns the unresolved symlink

  std::wstring const w = fs_win32_to_wide(path);

  DWORD const L = GetFullPathNameW(w.c_str(), 0, nullptr, nullptr);
  if(L > 0){
    std::wstring r(L, '\0');
    if(DWORD const Lr = GetFullPathNameW(w.c_str(), static_cast<DWORD>(r.size()), r.data(), nullptr); Lr < L && Lr > 0)
      return fs_win32_to_narrow(r);
    else
      fs_error_callback(path, "GetFullPathNameW: Lr=" + std::to_string(Lr));
  }

  fs_error_callback(path, "GetFullPathNameW: L=" + std::to_string(L));
  return {};
}


std::string fs_win32_final_path(std::string_view path)
{
  // resolves Windows symbolic links (reparse points and junctions)
  // it also resolves the case insensitivity of Windows paths to the disk case
  // PATH MUST EXIST
  //
  // References:
  // https://stackoverflow.com/a/50182947
  // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew
  // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfinalpathnamebyhandlew

  if(fs_trace) std::cout << "TRACE: win32_final_path(" << path << ")\n";
  // dwDesiredAccess=0 to allow getting parameters even without read permission

  HANDLE h = fs_win32_get_file_handle(path);
  if(h == INVALID_HANDLE_VALUE) {
    fs_error_callback(path, "CreateFileW failed");
    return {};
  }

  if(DWORD const L = GetFinalPathNameByHandleW(h, nullptr, 0, FILE_NAME_NORMALIZED | VOLUME_NAME_DOS); L > 0)
  {
    std::wstring w(L, '\0');
    DWORD const Lr = GetFinalPathNameByHandleW(h, w.data(), L, FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);

    CloseHandle(h);

    if(Lr == L-1) {
      std::string r(fs_win32_to_narrow(w));

      if (fs_win32_is_ext_path(r) && !fs_win32_is_ext_path(path))
        r.erase(0, 4);

      return r;
    }
  } else {
    CloseHandle(h);
  }

  fs_error_callback(path);
  return {};
}
#endif


namespace ffilesystem {

std::string longname(std::string_view in)
{
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getlongpathnamew
// the path must exist

  std::error_code ec;

#if defined(_WIN32) || defined(__CYGWIN__)
// size includes null terminator on 1st call, but does not include null terminator on 2nd call
  std::wstring const w = fs_win32_to_wide(in);
  DWORD const L = GetLongPathNameW(w.c_str(), nullptr, 0);

  if(L > 0){
    std::wstring out(L, '\0');
    if(GetLongPathNameW(w.c_str(), out.data(), static_cast<DWORD>(out.size())) == L-1)
      return fs_win32_to_narrow(out);
  }
#else
  ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_error_callback(in, ec);
  return {};
}


std::string shortname(std::string_view in)
{
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getshortpathnamew
// the path must exist

  std::error_code ec;

#if defined(_WIN32) || defined(__CYGWIN__)
// size includes null terminator on 1st call, but does not include null terminator on 2nd call
  std::wstring const w = fs_win32_to_wide(in);
  DWORD const L = GetShortPathNameW(w.c_str(), nullptr, 0);

  if(L > 0) {
    std::wstring out(L, '\0');
    if(GetShortPathNameW(w.c_str(), out.data(), static_cast<DWORD>(out.size())) == L-1)
      return fs_win32_to_narrow(out);
  }
#else
  ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_error_callback(in, ec);
  return {};
}

} // namespace ffilesystem


#if defined(_WIN32) || defined(__CYGWIN__)
std::string
fs_win32_to_narrow(std::wstring_view w)
{
  // https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte

  std::wstring ws(w);
  // assuming input is null-terminated and fourth argument is -1, then L includes the null terminator
  if (int L = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, nullptr, 0, nullptr, nullptr); L > 0)  FFS_LIKELY
  {
    std::string n(L, '\0');
    if(WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, n.data(), L, nullptr, nullptr) == L) {
      n.pop_back();
      return n;
    }
  }

  fs_error_callback("");
  return {};
}


std::wstring
fs_win32_to_wide(std::string_view n)
{
  // assuming input is null-terminated and fourth argument is -1, then L includes the null terminator
  // https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
  std::string ns(n);
  if (int L = MultiByteToWideChar(CP_UTF8, 0, ns.c_str(), -1, nullptr, 0); L > 0)  FFS_LIKELY
  {
    std::wstring w(L, '\0');
    if(MultiByteToWideChar(CP_UTF8, 0, ns.c_str(), -1, w.data(), L) == L) {
      w.pop_back();
      return w;
    }
  }

  fs_error_callback(n);
  return {};
}
#endif
