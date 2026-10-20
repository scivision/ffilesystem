#include "ffilesystem.h"

#include <cstdint> // uintmax_t
#include <system_error> // for error_code

#include <string_view>


#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Filesystem = std::filesystem;
#else

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // GetDiskFreeSpaceEx
#include "win32_path.h"
#else

#include <unistd.h>

#if __has_include(<sys/statvfs.h>)
#define HAVE_STATVFS
#include <sys/statvfs.h>
#endif

#endif

#endif


namespace {

struct space_info {
  std::uintmax_t available;
  std::uintmax_t capacity;
};

space_info get_space(std::string_view path)
{
  std::error_code ec;

#if defined(HAVE_CXX_FILESYSTEM)
  auto s = Filesystem::space(path, ec);
  if (ec) {
    fs_error_callback(path, ec);
    return {fs_unknown_size, fs_unknown_size};
  }

  return {s.available, s.capacity};
#elif defined(_WIN32)
  // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getdiskfreespaceexw
  if (ULARGE_INTEGER available, capacity;
      GetDiskFreeSpaceExW(fs_win32_to_wide(path).c_str(), &available, &capacity, nullptr) != 0)
    return {available.QuadPart, capacity.QuadPart};
#elif defined(HAVE_STATVFS)
  // https://www.man7.org/linux/man-pages/man3/statvfs.3.html
  // https://unix.stackexchange.com/a/703650
  if (struct statvfs stat; !::statvfs(std::string{path}.c_str(), &stat))
  {
    const auto block_size = stat.f_frsize ? stat.f_frsize : stat.f_bsize;
    return {block_size * stat.f_bavail, block_size * stat.f_blocks};
  }
#else
  ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_error_callback(path, ec);
  return {fs_unknown_size, fs_unknown_size};
}

}

std::uintmax_t fs_space_available(std::string_view path)
{
  // filesystem space available for device holding path
  return get_space(path).available;
}

std::uintmax_t fs_space_capacity(std::string_view path)
{
  // filesystem space capacity for device holding path
  // total size of the filesystem, in bytes
  // This is the quantity available to the non-privileged user,
  // not the total physical disk size.
  return get_space(path).capacity;
}
