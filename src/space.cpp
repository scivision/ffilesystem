#include "ffilesystem.h"

#include <cstdint> // uintmax_t
#include <system_error> // for error_code

#include <string_view>


#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#else
#include <string>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // GetDiskFreeSpaceEx
#else
#include <unistd.h>
#if __has_include(<sys/statvfs.h>)
#define HAVE_STATVFS
#include <sys/statvfs.h>
#endif
#endif
#endif

std::uintmax_t fs_space_available(std::string_view path)
{
  // filesystem space available for device holding path

  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM
  auto s = std::filesystem::space(path, ec);
  if (ec)
    fs_print_error(path, __func__, ec);

  return s.available;
#elif defined(_WIN32)
  // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getdiskfreespaceexa
  if(ULARGE_INTEGER b; GetDiskFreeSpaceExW(fs_win32_to_wide(path).data(), &b, nullptr, nullptr) != 0)
    return b.QuadPart;
#elif defined(HAVE_STATVFS)
  // https://www.man7.org/linux/man-pages/man3/statvfs.3.html
  // https://unix.stackexchange.com/a/703650
  struct statvfs stat;
  if (!statvfs(path.data(), &stat))  FFS_LIKELY
    return (stat.f_frsize ? stat.f_frsize : stat.f_bsize) * stat.f_bavail;
#else
  ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_print_error(path, __func__, ec);
  return static_cast<std::uintmax_t>(-1);
}


std::uintmax_t fs_space_capacity(std::string_view path)
{
  // filesystem space capacity for device holding path
  // total size of the filesystem, in bytes
  // This is the quantity available to the non-priviliged user,
  // not the total physical disk size.

  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM
  auto s = std::filesystem::space(path, ec);
  if (ec)
    fs_print_error(path, __func__, ec);

  return s.capacity;
#elif defined(_WIN32)
  if(ULARGE_INTEGER b; GetDiskFreeSpaceExW(fs_win32_to_wide(path).data(), nullptr, &b, nullptr) != 0)
    return b.QuadPart;
#elif defined(HAVE_STATVFS)
  struct statvfs stat;
  if (!statvfs(path.data(), &stat))  FFS_LIKELY
    return (stat.f_frsize ? stat.f_frsize : stat.f_bsize) * stat.f_blocks;
#else
  ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_print_error(path, __func__, ec);
  return static_cast<std::uintmax_t>(-1);
}
