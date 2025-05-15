#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winioctl.h>
#endif

#include <cstddef>  // for std::byte
#include <iostream>  // IWYU pragma: keep

#include <string>
#include <string_view>

#include <system_error>

#include "ffilesystem.h"

#if defined(_WIN32)
// create type PREPARSE_DATA_BUFFER
// from ntifs.h, which can only be used by drivers
// typedef is copied from https://gitlab.kitware.com/utils/kwsys/-/blob/master/SystemTools.cxx
// that has a BSD 3-clause license
typedef struct _REPARSE_DATA_BUFFER
{
  ULONG ReparseTag;
  USHORT ReparseDataLength;
  USHORT Reserved;
  union
  {
    struct
    {
      USHORT SubstituteNameOffset;
      USHORT SubstituteNameLength;
      USHORT PrintNameOffset;
      USHORT PrintNameLength;
      ULONG Flags;
      WCHAR PathBuffer[1];
    } SymbolicLinkReparseBuffer;
    struct
    {
      USHORT SubstituteNameOffset;
      USHORT SubstituteNameLength;
      USHORT PrintNameOffset;
      USHORT PrintNameLength;
      WCHAR PathBuffer[1];
    } MountPointReparseBuffer;
    struct
    {
      UCHAR DataBuffer[1];
    } GenericReparseBuffer;
    struct
    {
      ULONG Version;
      WCHAR StringList[1];
      // In version 3, there are 4 NUL-terminated strings:
      // * Package ID
      // * Entry Point
      // * Executable Path
      // * Application Type
    } AppExecLinkReparseBuffer;
  } DUMMYUNIONNAME;
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;
#endif


#if defined(_WIN32)
static bool fs_win32_get_reparse_buffer(std::string_view path, std::byte* buffer)
{

// this function is adapted from
// https://gitlab.kitware.com/utils/kwsys/-/blob/master/SystemTools.cxx
// that has a BSD 3-clause license

  std::error_code ec;


// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileattributesa

// https://learn.microsoft.com/en-us/windows/win32/fileio/file-attribute-constants
  if (DWORD attr = GetFileAttributesW(fs_win32_to_wide(path).data()); attr == INVALID_FILE_ATTRIBUTES)
    // ec = std::make_error_code(std::errc::no_such_file_or_directory);
    // don't emit error for non-existent files
    return false;
  else if (!(attr & FILE_ATTRIBUTE_REPARSE_POINT))
    return false;

  // Using 0 instead of GENERIC_READ as it allows reading of file attributes
  // even if we do not have permission to read the file itself

  // A reparse point may be an execution alias (Windows Store app), which
  // is similar to a symlink but it cannot be opened as a regular file.
  // We must look at the reparse point data explicitly.

  // FILE_ATTRIBUTE_REPARSE_POINT means:
  // * a file or directory that has an associated reparse point, or
  // * a file that is a symbolic link.

  HANDLE h = CreateFileW(fs_win32_to_wide(path).data(),
                         0, 0, nullptr, OPEN_EXISTING,
                         FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, nullptr);

  if (h == INVALID_HANDLE_VALUE)
    ec = std::make_error_code(std::errc::io_error);
  else {

    DWORD bytesReturned = 0;

    BOOL ok = DeviceIoControl(h, FSCTL_GET_REPARSE_POINT, nullptr, 0, buffer,
                          MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &bytesReturned,
                          nullptr);

    if(CloseHandle(h) && ok) FFS_LIKELY
      return true;
  }


  fs_print_error(path, __func__, ec);
  return false;
}
#endif


bool fs_is_appexec_alias(std::string_view path)
{
// Windows App Execution Alias allow easy access to Windows Store apps
// from the Windows Terminal. However, they don't work with _access_s()
// or stat() like regular files. This function detects the aliases.
// Reference:
// https://learn.microsoft.com/en-us/windows/terminal/command-line-arguments?tabs=windows#add-windows-terminal-executable-to-your-path
// https://learn.microsoft.com/en-us/windows/win32/fileio/reparse-points
//
// this function is adapted from
// https://gitlab.kitware.com/utils/kwsys/-/blob/master/SystemTools.cxx
// that has a BSD 3-clause license

#if defined(_WIN32)
  std::byte buffer[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];

  if (!fs_win32_get_reparse_buffer(path, buffer))
    return false;

  PREPARSE_DATA_BUFFER data =
    reinterpret_cast<PREPARSE_DATA_BUFFER>(&buffer[0]);

  // https://learn.microsoft.com/en-us/windows/win32/fileio/reparse-point-tags
  return data->ReparseTag == IO_REPARSE_TAG_APPEXECLINK;

#else
  fs_print_error(path, __func__, std::make_error_code(std::errc::function_not_supported));
  return false;
#endif

}


bool fs_win32_is_symlink(std::string_view path)
{
// distinguish between Windows symbolic links and reparse points as
// reparse points can be unlike symlinks.
//
// this function is adapted from
// https://gitlab.kitware.com/utils/kwsys/-/blob/master/SystemTools.cxx
// that has a BSD 3-clause license

#if defined(_WIN32)
  std::byte buffer[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
  // Since FILE_ATTRIBUTE_REPARSE_POINT is set this file must be
  // a symbolic link if it is not a reparse point.
  if (!fs_win32_get_reparse_buffer(path, buffer))
    return GetLastError() == ERROR_NOT_A_REPARSE_POINT;

  ULONG reparseTag =
    reinterpret_cast<PREPARSE_DATA_BUFFER>(&buffer[0])->ReparseTag;

  return (reparseTag == IO_REPARSE_TAG_SYMLINK) ||
         (reparseTag == IO_REPARSE_TAG_MOUNT_POINT);
#else
  fs_print_error(path, __func__, std::make_error_code(std::errc::function_not_supported));
  return false;
#endif

}


std::string fs_win32_full_name(std::string_view path)
{
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfullpathnameA
// GetFinalPathNameByHandle or GetFullPathName returns the unresolved symlink

  std::error_code ec;

#if defined(_WIN32)
  std::wstring const w = fs_win32_to_wide(path);

  auto const L = GetFullPathNameW(w.data(), 0, nullptr, nullptr);
  // this form includes the null terminator
  // weak detection of race condition (cwd change)
  if(L){
    std::wstring r;
    r.resize(L);
    if(GetFullPathNameW(w.data(), L, r.data(), nullptr) == L-1)  FFS_LIKELY
    {
      r.resize(L-1);
      return fs_win32_to_narrow(r);
    }
  }
#else
  ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_print_error(path, __func__, ec);
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
  // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
  // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfinalpathnamebyhandlea

  std::error_code ec;

#if defined(_WIN32)
  if(fs_trace) std::cout << "TRACE: win32_final_path(" << path << ")\n";
  // dwDesiredAccess=0 to allow getting parameters even without read permission
  // FILE_FLAG_BACKUP_SEMANTICS required to open a directory

  std::wstring w = fs_win32_to_wide(path);

  HANDLE h = CreateFileW(w.data(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                         OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
  if(h == INVALID_HANDLE_VALUE)
    return {};

  if(DWORD L = GetFinalPathNameByHandleW(h, nullptr, 0, FILE_NAME_NORMALIZED | VOLUME_NAME_DOS); L > 0) {
    w.resize(L + 1);

    L = GetFinalPathNameByHandleW(h, w.data(), L, FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);
    if(CloseHandle(h) && L > 0) {
      w.resize(L);
      std::string r(fs_win32_to_narrow(w));

      if (r.substr(0, 4) == R"(\\?\)" && path.substr(0, 4) != R"(\\?\)")
        r = r.substr(4);

      return r;
    }
  }
#else
  ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_print_error(path, __func__, ec);
  return {};
}


std::string fs_longname(std::string_view in)
{
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getlongpathnamea
// the path must exist

  std::error_code ec;

#if defined(_WIN32) || defined(__CYGWIN__)
// size includes terminal on 1st call, but does not include null terminator on 2nd call
  std::wstring const w = fs_win32_to_wide(in);
  DWORD L = GetLongPathNameW(w.data(), nullptr, 0);

  if(L > 0){
    std::wstring out;
    out.resize(L);

    if(GetLongPathNameW(w.data(), out.data(), L) == L-1) {
      out.resize(L);
      return fs_win32_to_narrow(out);
    }
  }
#else
  ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_print_error(in, __func__, ec);
  return {};
}


std::string fs_shortname(std::string_view in)
{
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getshortpathnamew
// the path must exist

  std::error_code ec;

#if defined(_WIN32) || defined(__CYGWIN__)
// size includes terminal on 1st call, but does not include null terminator on 2nd call
  std::wstring const w = fs_win32_to_wide(in);
  DWORD L = GetShortPathNameW(w.data(), nullptr, 0);

  if(L > 0){
    std::wstring out;
    out.resize(L);

    if(GetShortPathNameW(w.data(), out.data(), L) == L-1) {
      out.resize(L);
      return fs_win32_to_narrow(out);
    }
  }
#else
  ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_print_error(in, __func__, ec);
  return {};
}


std::string fs_win32_to_narrow([[maybe_unused]] std::wstring_view w)
{
  std::error_code ec;

#if defined(_WIN32)
  if (int L = WideCharToMultiByte(CP_UTF8, 0, w.data(), -1, nullptr, 0, nullptr, nullptr); L > 0)  FFS_LIKELY
  {
    std::string n;
    n.resize(L);

    if(WideCharToMultiByte(CP_UTF8, 0, w.data(), -1, n.data(), L, nullptr, nullptr) == L) {
      n.resize(L-1);  // discard null terminator
      return n;
    }
  }
#else
  ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_print_error("", __func__, ec);
  return {};
}


std::wstring fs_win32_to_wide(std::string_view n)
{
  std::error_code ec;

#if defined(_WIN32)
  // https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar

  if (int L = MultiByteToWideChar(CP_UTF8, 0, n.data(), -1, nullptr, 0); L > 0)  FFS_LIKELY
  {
    std::wstring w;
    w.resize(L);

    if(MultiByteToWideChar(CP_UTF8, 0, n.data(), -1, w.data(), L) == L) {
      w.resize(L-1);  // discard null terminator
      return w;
    }
  }
#else
  ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_print_error(n, __func__, ec);
  return {};
}
