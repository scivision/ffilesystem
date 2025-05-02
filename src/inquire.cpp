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
#include <winioctl.h> // DeviceIoControl
#include <io.h> // _access_s
#endif

#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
#elif !defined(_WIN32)
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

// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
  HANDLE h = CreateFileA(path.data(), 0, 0, nullptr, OPEN_EXISTING, 0, nullptr);

  if(h == INVALID_HANDLE_VALUE){
    DWORD err = GetLastError();
    switch (err) {
      case ERROR_CANT_ACCESS_FILE: case ERROR_FILE_NOT_FOUND: case ERROR_PATH_NOT_FOUND: case ERROR_SUCCESS:
        return false;
      default:
        ec = std::make_error_code(std::errc::io_error);
    }

    fs_print_error(path, __func__);
    return false;
  }

// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfiletype
  DWORD t = GetFileType(h);
  if (CloseHandle(h) && !ec)
    return t == type;

  fs_print_error(path, __func__, ec);
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
  int r = 0;
#if defined(STATX_MODE) && defined(USE_STATX)
// Linux Glibc only
// https://www.gnu.org/software/gnulib/manual/html_node/statx.html
// https://www.man7.org/linux/man-pages/man2/statx.2.html

  struct statx sx;
  r = statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_MODE, &sx);
  if (r == 0) FFS_LIKELY
    return sx.stx_mode;
#endif

// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/stat-functions
  if(r == 0 || errno == ENOSYS){
    if (struct stat s; !stat(path.data(), &s))
      return s.st_mode;
  }

  return 0;
}


bool
fs_is_removable(std::string_view path)
{
  // is path a removable device like a USB stick or SD card or CD-ROM, DVD, Blu-ray
  // not a fixed disk like a hard drive or SSD
#if defined(_WIN32)
  // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getdrivetypea
  UINT t = GetDriveTypeA(fs_root(path).data());
  switch (t)
  {
    case DRIVE_REMOVABLE:
    case DRIVE_CDROM:
      return true;
    case DRIVE_FIXED:
    case DRIVE_REMOTE:
    case DRIVE_RAMDISK:
      return false;
    case DRIVE_UNKNOWN:
    case DRIVE_NO_ROOT_DIR:
      fs_print_error(path, __func__, std::make_error_code(std::errc::no_such_device));
      return false;
    default:
      return false;
  }
#else
  // Linux: find the device and check /sys/block/*/removable == 1
  // macOS: check /Volumes/*/ for a removable device
  fs_print_error(path, __func__, std::make_error_code(std::errc::function_not_supported));
  return false;
#endif

}


bool
fs_exists(std::string_view path)
{
  // fs_exists() is true even if path is non-readable
  // this is like Python pathlib.Path.exists()
  // unlike kwSys:SystemTools:FileExists which uses R_OK instead of F_OK like this project.

  bool ok;
#if defined(HAVE_CXX_FILESYSTEM)
  std::error_code ec;
  ok = (std::filesystem::exists(path, ec) && !ec) ||
        (fs_is_msvc() && fs_is_appexec_alias(path));
#elif defined(_WIN32)
  WIN32_FILE_ATTRIBUTE_DATA fad;
  ok = GetFileAttributesExA(path.data(), GetFileExInfoStandard, &fad);
#else
  // unistd.h
  ok = !access(path.data(), F_OK);
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
  ok = std::filesystem::is_directory(path, ec) && !ec;
#elif defined(_WIN32)
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileattributesexa
  WIN32_FILE_ATTRIBUTE_DATA fad;
  ok = GetFileAttributesExA(path.data(), GetFileExInfoStandard, &fad) &&
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
  ok = (std::filesystem::is_regular_file(path, ec) && !ec) ||
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
  ok = std::filesystem::is_fifo(path, ec) && !ec;
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
  ok = std::filesystem::is_character_file(path, ec) && !ec;
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
#elif defined(_WIN32)
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

  auto s = std::filesystem::hard_link_count(path, ec);
  if(ec)
    fs_print_error(path, __func__, ec);

  return s;

#else

  int r = 0;

#if defined(STATX_NLINK) && defined(USE_STATX)
// https://www.man7.org/linux/man-pages/man2/statx.2.html
  struct statx sx;
  r = statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_NLINK, &sx);
  if (r == 0) FFS_LIKELY
    return sx.stx_nlink;
#endif

  if (r == 0 || errno == ENOSYS){
    if (struct stat s; !stat(path.data(), &s))
      return s.st_nlink;
  }

  fs_print_error(path, __func__, ec);
  return static_cast<std::uintmax_t>(-1);
#endif
}


std::size_t fs_get_blksize(std::string_view path)
{
  // block size in bytes
#if defined(_WIN32)

  const std::string root = fs_root_name(path);
  if (root.empty())
    return {};

  const std::string wp = R"(\\.\)" + root;

  HANDLE h = CreateFileA(wp.data(), 0, 0, nullptr, OPEN_EXISTING, 0, nullptr);
  if (h != INVALID_HANDLE_VALUE) {
    DISK_GEOMETRY_EX diskGeometry = {};
    ZeroMemory(&diskGeometry, sizeof(DISK_GEOMETRY_EX));
    DWORD bytesReturned = 0;

    bool const ok = DeviceIoControl(h, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
                        nullptr, 0, &diskGeometry, sizeof(diskGeometry),
                        &bytesReturned, nullptr);

    if (CloseHandle(h) && ok)  FFS_LIKELY
      return diskGeometry.Geometry.BytesPerSector;
  }

#else

  int r = 0;

#if defined(STATX_BASIC_STATS) && defined(USE_STATX)
  struct statx sx;
  r = statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW, STATX_BASIC_STATS, &sx);
  if (r == 0) FFS_LIKELY
    return sx.stx_blksize;
#endif

  if (r == 0 || errno == ENOSYS){
    if (struct stat s; !stat(path.data(), &s))
      return s.st_blksize;
  }
#endif

  fs_print_error(path, __func__);
  return {};
}
