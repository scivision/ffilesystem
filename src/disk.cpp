#include <string>
#include <system_error>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winioctl.h> // DeviceIoControl
#endif

#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(__linux__)
#include <sys/sysmacros.h> // for makedev()

#if defined(USE_STATX)
#include <fcntl.h>   // AT_* constants for statx()
#endif
#endif

#include "ffilesystem.h"


std::size_t fs_get_blksize(std::string_view path)
{
  // block size in bytes
#if defined(_WIN32)

  const std::string root = fs_root_name(path);
  if (root.empty())
    return {};

  HANDLE h = CreateFileW(fs_win32_to_wide(R"(\\.\)" + root).data(),
                         0,
                         FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                         nullptr, OPEN_EXISTING, 0, nullptr);

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

#else

  int r = 0;

#if defined(STATX_BASIC_STATS) && defined(USE_STATX)
  struct statx sx;
  r = statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW, STATX_BASIC_STATS, &sx);
  if (r == 0)
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


dev_t fs_st_dev(std::string_view path)
{
  // device number of the file or directory

  int r = 0;

#if defined(STATX_INO) && defined(USE_STATX)

  struct statx x;
  r = statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_INO, &x);
  if (r == 0)
    return makedev(x.stx_dev_major, x.stx_dev_minor);

#endif

  if (r == 0 || errno == ENOSYS){
    if(struct stat s; stat(path.data(), &s) == 0)
      return s.st_dev;
  }

  fs_print_error(path, __func__);
  return -1;
}


ino_t fs_inode(std::string_view path)
{
  // inode number of the file or directory
  //
  // Windows: .st_ino is always zero.
  // See source code for fs_equivalent() for how to use BY_HANDLE_FILE_INFORMATION
  // with GetFileInformationByHandle().

  std::error_code ec;

#if defined(_WIN32)

#if defined(HAVE_GETFILEINFORMATIONBYNAME)
  // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-file_stat_basic_information
  FILE_STAT_BASIC_INFORMATION f1;
  const auto w1 = fs_win32_to_wide(path);

  if (GetFileInformationByName(w1.data(), FileStatBasicByNameInfo, &f1, sizeof(f1)))
    return f1.FileId.QuadPart;

#else
  ec = std::make_error_code(std::errc::function_not_supported);
#endif

#else

  int r = 0;

#if defined(STATX_INO) && defined(USE_STATX)

  struct statx x;
  r = statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_INO, &x);
  if (r == 0)
    return x.stx_ino;

#endif

  if (r == 0 || errno == ENOSYS) {
    if(struct stat s; stat(path.data(), &s) == 0)
      return s.st_ino;
  }

#endif // _WIN32

  fs_print_error(path, __func__, ec);
  return 0;

}
