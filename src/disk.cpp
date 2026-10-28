#if defined(__linux__) || defined(__CYGWIN__)
#if !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif
#endif

#include <string>
#include <system_error>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winioctl.h> // DeviceIoControl
#include "win32_path.h"
#endif

#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(HAVE_STATX)
#include <sys/sysmacros.h> // for makedev
#include <fcntl.h>   // AT_* constants for statx
#endif

#include "ffilesystem.h"
#include "internal.h"


namespace ffilesystem {

std::size_t get_blksize(std::string_view path)
{
  // block size in bytes

#if defined(_WIN32)
  const std::string root = root_name(path);
  if (root.empty())
    return {};

  HANDLE h = fs_win32_get_file_handle(R"(\\.\)" + root);

  if (h != INVALID_HANDLE_VALUE) {
    DISK_GEOMETRY_EX diskGeometry = {};
    ZeroMemory(&diskGeometry, sizeof(DISK_GEOMETRY_EX));
    DWORD bytesReturned = 0;

    bool const ok = DeviceIoControl(h, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
                        nullptr, 0, &diskGeometry, sizeof(diskGeometry),
                        &bytesReturned, nullptr);

    CloseHandle(h);
    if (ok)
      return diskGeometry.Geometry.BytesPerSector;
  }

  fs_error_callback(path);
  return std::size_t{};
#else

#if defined(HAVE_STATX)
  return fs_internal::get_stat_value<std::size_t>(
      path, {}, fs_internal::report_stat_error, STATX_BASIC_STATS,
      [](const struct statx& x) { return static_cast<std::size_t>(x.stx_blksize); },
      [](const struct stat& s) { return static_cast<std::size_t>(s.st_blksize); });
#else
  return fs_internal::get_stat_value<std::size_t>(
      path, {}, fs_internal::report_stat_error,
      [](const struct stat& s) { return static_cast<std::size_t>(s.st_blksize); });
#endif

#endif
}


dev_t st_dev(std::string_view path)
{
  // device number of the file or directory
  // in general dev_t may be unsigned.

#if defined(HAVE_STATX)
  // don't call as ::makedev because some platforms e.g. Android have makedev as a macro
  return fs_internal::get_stat_value<dev_t>(
      path, {}, fs_internal::report_stat_error, STATX_INO,
      [](const struct statx& x) { return makedev(x.stx_dev_major, x.stx_dev_minor); },
      [](const struct stat& s) { return s.st_dev; });
#else
  return fs_internal::get_stat_value<dev_t>(
      path, {}, fs_internal::report_stat_error, [](const struct stat& s) { return s.st_dev; });
#endif
}


ino_t inode(std::string_view path)
{
  // inode number of the file or directory
  //
  // Windows: .st_ino is always zero.

  auto handle_error = [&]() {
    fs_error_callback(path);
    return ino_t{};
  };

#if defined(_WIN32)
// we don't use GetFileInformationByName because it has trouble with relative symlinks, and
// we don't want to confuse the inode number with a potentially incorrect file ID

  HANDLE h = fs_win32_get_file_handle(path);
  if (h == INVALID_HANDLE_VALUE)
    return handle_error();

  BY_HANDLE_FILE_INFORMATION info{};
  const BOOL ok = GetFileInformationByHandle(h, &info);
  CloseHandle(h);

  if (ok)
    return static_cast<ino_t>((static_cast<std::uint64_t>(info.nFileIndexHigh) << 32) |
                             info.nFileIndexLow);

#else

#if defined(HAVE_STATX)
  return fs_internal::get_stat_value<ino_t>(
      path, {}, fs_internal::report_stat_error, STATX_INO,
      [](const struct statx& x) { return static_cast<ino_t>(x.stx_ino); },
      [](const struct stat& s) { return s.st_ino; });
#else
  return fs_internal::get_stat_value<ino_t>(
      path, {}, fs_internal::report_stat_error, [](const struct stat& s) { return s.st_ino; });
#endif

#endif // _WIN32

  return handle_error();

}

}
