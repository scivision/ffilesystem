// IWYU pragma: no_include <bits/statx-generic.h>
// IWYU pragma: no_include <linux/stat.h>

#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include "ffilesystem.h"

#include <string_view>
#include <system_error>
#include <iostream>  // IWYU pragma: keep
#include <cstdint> // uintmax_t

// include even if <filesystem> is available
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <io.h> // _access_s
#endif

#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Filesystem = std::filesystem;
#elif !defined(_WIN32)
#include <unistd.h>
#endif

#include <sys/types.h>  // IWYU pragma: keep
#include <sys/stat.h>   // IWYU pragma: keep

#if __has_include(<fcntl.h>)
#include <fcntl.h>   // AT_* constants for statx
#endif



#if defined(_WIN32)
static bool fs_win32_is_type(std::string_view path, const DWORD type){

// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
  HANDLE h = CreateFileW(fs_win32_to_wide(path).data(),
                         0,
                         FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                         nullptr, OPEN_EXISTING, 0, nullptr);

  if(h == INVALID_HANDLE_VALUE){
    DWORD err = GetLastError();
    switch (err) {
      case ERROR_CANT_ACCESS_FILE: case ERROR_FILE_NOT_FOUND: case ERROR_PATH_NOT_FOUND: case ERROR_SUCCESS:
        return false;
      default:
        fs_print_error(path, __func__);
        return false;
    }
  }

// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfiletype
  DWORD t = GetFileType(h);
  CloseHandle(h);
  if (t != FILE_TYPE_UNKNOWN)
    return t == type;

  fs_print_error(path, __func__);
  return false;
}
#endif


bool fs_has_statx()
{
// https://www.man7.org/linux/man-pages/man2/statx.2.html
#if defined(HAVE_STATX)
  return true;
#else
  return false;
#endif
}


mode_t
fs_st_mode(std::string_view path)
{
#if defined(HAVE_STATX)
// Linux Glibc only
// https://www.gnu.org/software/gnulib/manual/html_node/statx.html
// https://www.man7.org/linux/man-pages/man2/statx.2.html

  struct statx x;
  if (::statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_MODE, &x) == 0) {
    return x.stx_mode;
  } else if (errno != ENOSYS) {
    return 0;
  }
#endif

// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/stat-functions
  if (struct stat s; ::stat(path.data(), &s) == 0)
    return s.st_mode;

  return 0;
}


bool
fs_exists(std::string_view path)
{
  // fs_exists() is true even if path is non-readable, as long as UID has permission to see it.
  // this is like Python pathlib.Path.exists()
  // unlike kwSys:SystemTools:FileExists which uses R_OK instead of F_OK like this project.

  bool ok;
#if defined(HAVE_CXX_FILESYSTEM)
  std::error_code ec;
  ok = (Filesystem::exists(path, ec) && !ec) ||
        (fs_is_msvc() && fs_is_appexec_alias(path));
  if (ec && ec != std::errc::no_such_file_or_directory)
    fs_print_error(path, __func__, ec);
#elif defined(_WIN32)
  WIN32_FILE_ATTRIBUTE_DATA fad;

  ok = GetFileAttributesExW(fs_win32_to_wide(path).data(), GetFileExInfoStandard, &fad);
  if (!ok){
    DWORD err = GetLastError();
    if (err != ERROR_FILE_NOT_FOUND && err != ERROR_PATH_NOT_FOUND)
      fs_print_error(path, __func__);
  }
#else
  // https://www.man7.org/linux/man-pages/man2/access.2.html
  ok = access(path.data(), F_OK) == 0;
  if (!ok && errno != ENOENT && errno != ENOTDIR)
    fs_print_error(path, __func__);
#endif

  return ok;
}


bool
fs_is_dir(std::string_view path)
{
  // is path a directory or a symlink to a directory

  bool ok;
#if defined(HAVE_CXX_FILESYSTEM)
// NOTE: Windows top-level drive "C:" needs a trailing slash "C:/"
  std::error_code ec;
  ok = Filesystem::is_directory(path, ec);
  if (ec && ec != std::errc::no_such_file_or_directory)
    fs_print_error(path, __func__, ec);
#elif defined(_WIN32)
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileattributesexa
// this also works for Symlinks to directories
  WIN32_FILE_ATTRIBUTE_DATA fad;

  ok = GetFileAttributesExW(fs_win32_to_wide(path).data(), GetFileExInfoStandard, &fad) &&
       (fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
#else
  ok = S_ISDIR(fs_st_mode(path));
#endif

  return ok;
}


bool
fs_is_file(std::string_view path)
{
  // is path a regular file or a symlink to a regular file.
  // not a directory, device, or symlink to a directory.
  // stat() doesn't detect App Execution Aliases
  // AppExec Alias have FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_ARCHIVE
  // but need to check the reparse point data for IO_REPARSE_TAG_APPEXECLINK

  bool ok;
#if defined(HAVE_CXX_FILESYSTEM)
  std::error_code ec;
  ok = (Filesystem::is_regular_file(path, ec) && !ec) ||
        (fs_is_msvc() && fs_is_appexec_alias(path));
#elif defined(_WIN32)
  ok = fs_win32_is_type(path, FILE_TYPE_DISK) || fs_is_appexec_alias(path);
#else
  ok = S_ISREG(fs_st_mode(path));
#endif

  return ok;
}


bool
fs_is_fifo(std::string_view path)
{
  // mkfifo() or CreateNamedPipe()
  bool ok;

#if defined(_WIN32)
  ok = fs_win32_is_type(path, FILE_TYPE_PIPE);
#elif defined(HAVE_CXX_FILESYSTEM)
  std::error_code ec;
  ok = Filesystem::is_fifo(path, ec) && !ec;
#else
  ok = S_ISFIFO(fs_st_mode(path));
#endif

  return ok;
}


bool fs_is_char_device(std::string_view path)
{
// character device like /dev/null or CONIN$

  bool ok;
#if defined(_WIN32)
// currently broken in MSVC STL and MinGW Clang ARM for <filesystem>
  ok = fs_win32_is_type(path, FILE_TYPE_CHAR);
#elif defined(HAVE_CXX_FILESYSTEM)
  std::error_code ec;
  ok = Filesystem::is_character_file(path, ec) && !ec;
#else
  // Windows: https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fstat-fstat32-fstat64-fstati64-fstat32i64-fstat64i32
  ok = S_ISCHR(fs_st_mode(path));
#endif

  return ok;
}


bool fs_is_other(std::string_view path)
{
  // just as defined by <filesystem>
  // note that the symlink could point to something not a file or directory
  // https://en.cppreference.com/w/cpp/filesystem/is_other
  return fs_exists(path) && !fs_is_file(path) && !fs_is_dir(path) && !fs_is_symlink(path);
}


bool fs_is_readable(std::string_view path)
{
  // is path readable by the user
  // does not guarantee that the path can be opened (for example, it may be locked)

#if defined(_WIN32)
  // MSVC / MinGW ::perms doesn't detect App Execution Aliases readability
  // otherwise ::filesystem works for Windows ::perms,
  // but to be most efficient and deduplicate code, we implement like this.
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/access-s-waccess-s
  return _access_s(path.data(), 4) == 0;

#elif defined(HAVE_CXX_FILESYSTEM)

  std::error_code ec;
  const auto s = Filesystem::status(path, ec);

  if(ec || !Filesystem::exists(s))
    return false;

#if defined(__cpp_using_enum)  // C++20
  using enum Filesystem::perms;
#else
  constexpr Filesystem::perms none = Filesystem::perms::none;
  constexpr Filesystem::perms owner_read = Filesystem::perms::owner_read;
  constexpr Filesystem::perms group_read = Filesystem::perms::group_read;
  constexpr Filesystem::perms others_read = Filesystem::perms::others_read;
#endif

  return (s.permissions() & (owner_read | group_read | others_read)) != none;

#else
  return access(path.data(), R_OK) == 0;
#endif
}


bool fs_is_writable(std::string_view path)
{
  // is path writable by the user
  // does not guarantee that the path can be opened (for example, it may be locked)

#if defined(HAVE_CXX_FILESYSTEM)
  std::error_code ec;
  const auto s = Filesystem::status(path, ec);

  if(ec || !Filesystem::exists(s))
    return false;

#if defined(__cpp_using_enum)  // C++20
  using enum Filesystem::perms;
#else
  constexpr Filesystem::perms owner_write = Filesystem::perms::owner_write;
  constexpr Filesystem::perms group_write = Filesystem::perms::group_write;
  constexpr Filesystem::perms others_write = Filesystem::perms::others_write;
  constexpr Filesystem::perms none = Filesystem::perms::none;
#endif

  return (s.permissions() & (owner_write | group_write | others_write)) != none;
#elif defined(_WIN32)
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/access-s-waccess-s
  return _access_s(path.data(), 2) == 0;
#else
  return access(path.data(), W_OK) == 0;
#endif
}


std::uintmax_t fs_hard_link_count(std::string_view path)
{
  std::error_code ec;

#if defined(HAVE_CXX_FILESYSTEM)

  auto s = Filesystem::hard_link_count(path, ec);
  if(ec)
    fs_print_error(path, __func__, ec);

  return s;

#else

  int r = 0;

#if defined(HAVE_STATX)
// https://www.man7.org/linux/man-pages/man2/statx.2.html
  struct statx sx;
  r = ::statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_NLINK, &sx);
  if (r == 0)
    return sx.stx_nlink;
#endif

  if (r == 0 || errno == ENOSYS){
    if (struct stat s; !::stat(path.data(), &s))
      return s.st_nlink;
  }

  fs_print_error(path, __func__, ec);
  return static_cast<std::uintmax_t>(-1);
#endif
}
