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
#elif !defined(_MSC_VER)
#include <unistd.h>
#endif

#include <sys/types.h>  // IWYU pragma: keep
#include <sys/stat.h>   // IWYU pragma: keep

#if defined(__linux__) && defined(USE_STATX)
#include <fcntl.h>   // AT_* constants for statx()
#endif


#if defined(_WIN32)
bool fs_win32_is_type(std::string_view path, const DWORD type){

  std::error_code ec;

  HANDLE h = CreateFileA(path.data(), GENERIC_READ, FILE_SHARE_READ,
                nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);

  if(h == INVALID_HANDLE_VALUE)
    ec = std::make_error_code(std::errc::no_such_file_or_directory);
  else {
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfiletype
    DWORD t = GetFileType(h);
    if(!CloseHandle(h))
      ec = std::make_error_code(std::errc::io_error);
    if (!ec)
      return t == type;
  }

  fs_print_error(path, "win32_is_type", ec);
  return false;
}
#endif


bool fs_has_statx()
{
// https://www.man7.org/linux/man-pages/man2/statx.2.html
#if defined(STATX_MODE)
  return true;
#else
  return false;
#endif
}


int
fs_st_mode(std::string_view path)
{
#if defined(STATX_MODE) && defined(USE_STATX)
// Linux Glibc only
// https://www.gnu.org/software/gnulib/manual/html_node/statx.html
// https://www.man7.org/linux/man-pages/man2/statx.2.html

  if(struct statx s;
      statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_MODE, &s) == 0)
    return s.stx_mode;
#else
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/stat-functions
  if(struct stat s;
      !stat(path.data(), &s))
    return s.st_mode;
#endif
  // fs_print_error(path, "st_mode");
  return 0;
}


bool
fs_exists(std::string_view path)
{
  // fs_exists() is true even if path is non-readable
  // this is like Python pathlib.Path.exists()
  // unlike kwSys:SystemTools:FileExists which uses R_OK instead of F_OK like this project.
  // MSVC / MinGW std::filesystem::exists and _access_s don't detect App Execution Aliases

  bool ok;
#if defined(HAVE_CXX_FILESYSTEM)
  std::error_code ec;
  ok = (std::filesystem::exists(path, ec) && !ec);
#elif defined(_MSC_VER)
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/access-s-waccess-s
  ok = _access_s(path.data(), 0) == 0;
#else
  // unistd.h
  ok = !access(path.data(), F_OK);
#endif

  return ok || (fs_is_windows() && fs_is_appexec_alias(path));
}


bool
fs_is_dir(std::string_view path)
{
  // is path a directory or a symlink to a directory

  bool ok;
#if defined(HAVE_CXX_FILESYSTEM)
// NOTE: Windows top-level drive "C:" needs a trailing slash "C:/"
  std::error_code ec;
  ok = std::filesystem::is_directory(path, ec) && !ec;
#elif defined(_WIN32)
// stat() & S_IFDIR works on Windows, but we use GetFileAttributesA for didactic reasons
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileattributesa
  const DWORD attr = GetFileAttributesA(path.data());
  ok = attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY);
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
  // MSVC / MinGW std::filesystem::is_regular_file and stat() don't detect App Execution Aliases

  bool ok;
#if defined(HAVE_CXX_FILESYSTEM)
  std::error_code ec;
  ok = std::filesystem::is_regular_file(path, ec) && !ec;
#else
  ok = fs_st_mode(path) & S_IFREG;
  // S_ISREG not available with MSVC
#endif

  return ok || (fs_is_windows() && fs_is_appexec_alias(path));
}


bool
fs_is_fifo(std::string_view path)
{

  bool ok;
#if defined(HAVE_CXX_FILESYSTEM)
  std::error_code ec;
  ok = std::filesystem::is_fifo(path, ec) && !ec;
#elif defined(_MSC_VER)
  ok = fs_win32_is_type(path, FILE_TYPE_PIPE);
#else
  ok = S_ISFIFO(fs_st_mode(path));
#endif

  return ok;
}


bool fs_is_char_device(std::string_view path)
{
// character device like /dev/null or CONIN$

  bool ok;
#if defined(WIN32)
// currently broken in MSVC STL and MinGW Clang ARM for <filesystem>
  ok = fs_win32_is_type(path, FILE_TYPE_CHAR);
#elif defined(HAVE_CXX_FILESYSTEM)
  std::error_code ec;
  ok = std::filesystem::is_character_file(path, ec) && !ec;
#else
  // Windows: https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fstat-fstat32-fstat64-fstati64-fstat32i64-fstat64i32
  ok = S_ISCHR(fs_st_mode(path));
#endif

  return ok;
}


bool fs_is_exe(std::string_view path)
{
  // is path (file or symlink to a file) executable by the user
  // directories are not considered executable--use fs_get_permissions() for that.

  if(!fs_is_file(path))
    return false;

#if defined(_WIN32)
  // on Windows, std::filesystem isn't well-suited for executable file detection
  // https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getbinarytypea
  // GetBinaryTypeA(path.data(), nullptr) != 0 crashes with -1073741819 on MinGW GCC 14.2.0
  // MSVC, Windows oneAPI need is_appexec_alias
  return (fs_st_mode(path) & _S_IEXEC) || fs_is_appexec_alias(path);

#elif defined(HAVE_CXX_FILESYSTEM)

#if defined(__cpp_using_enum)  // C++20
  using enum std::filesystem::perms;
#else
  constexpr std::filesystem::perms none = std::filesystem::perms::none;
  constexpr std::filesystem::perms others_exec = std::filesystem::perms::others_exec;
  constexpr std::filesystem::perms group_exec = std::filesystem::perms::group_exec;
  constexpr std::filesystem::perms owner_exec = std::filesystem::perms::owner_exec;
#endif

  std::error_code ec;
  const auto s = std::filesystem::status(path, ec);

  return !ec && (s.permissions() & (owner_exec | group_exec | others_exec)) != none;

#else
  return access(path.data(), X_OK) == 0;
#endif
}


bool fs_is_readable(std::string_view path)
{
  // is directory or file readable by the user

#if defined(_WIN32)
  // MSVC / MinGW ::perms doesn't detect App Execution Aliases readability
  // otherwise ::filesystem works for Windows ::perms,
  // but to be most efficient and deduplicate code, we implement like this.
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/access-s-waccess-s
  return _access_s(path.data(), 4) == 0;

#elif defined(HAVE_CXX_FILESYSTEM)

  std::error_code ec;
  const auto s = std::filesystem::status(path, ec);
  if(ec || !std::filesystem::exists(s))
    return false;

#if defined(__cpp_using_enum)  // C++20
  using enum std::filesystem::perms;
#else
  constexpr std::filesystem::perms none = std::filesystem::perms::none;
  constexpr std::filesystem::perms owner_read = std::filesystem::perms::owner_read;
  constexpr std::filesystem::perms group_read = std::filesystem::perms::group_read;
  constexpr std::filesystem::perms others_read = std::filesystem::perms::others_read;
#endif

  return (s.permissions() & (owner_read | group_read | others_read)) != none;

#else
  return !access(path.data(), R_OK);
#endif
}


bool fs_is_writable(std::string_view path)
{
  // directory or file writable
#if defined(HAVE_CXX_FILESYSTEM)
  std::error_code ec;
  const auto s = std::filesystem::status(path, ec);
  if(ec || !std::filesystem::exists(s))
    return false;

#if defined(__cpp_using_enum)  // C++20
  using enum std::filesystem::perms;
#else
  constexpr std::filesystem::perms owner_write = std::filesystem::perms::owner_write;
  constexpr std::filesystem::perms group_write = std::filesystem::perms::group_write;
  constexpr std::filesystem::perms others_write = std::filesystem::perms::others_write;
  constexpr std::filesystem::perms none = std::filesystem::perms::none;
#endif

  return (s.permissions() & (owner_write | group_write | others_write)) != none;
#elif defined(_MSC_VER)
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/access-s-waccess-s
  return !_access_s(path.data(), 2);
#else
  return !access(path.data(), W_OK);
#endif
}


std::uintmax_t fs_hard_link_count(std::string_view path)
{
  std::error_code ec;

#if defined(HAVE_CXX_FILESYSTEM)

  if(auto s = std::filesystem::hard_link_count(path, ec); !ec)  FFS_LIKELY
    return s;

#elif defined(STATX_BASIC_STATS) && defined(USE_STATX)
// https://www.man7.org/linux/man-pages/man2/statx.2.html
  if (fs_trace) std::cout << "TRACE: statx() hard_link " << path << "\n";
  struct statx s;

  if( statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_BASIC_STATS, &s) == 0 ) FFS_LIKELY
    return s.stx_nlink;

#else
  if (struct stat s;
        !stat(path.data(), &s))
    return s.st_nlink;

#endif

  fs_print_error(path, "hard_link_count", ec);
  return {};
}
