#if defined(__linux__) || defined(__CYGWIN__)
#if !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif
#endif

#include "ffilesystem.h"

#include <string_view>
#include <system_error>
#include <cerrno>
#include <iostream>  // IWYU pragma: keep

#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Filesystem = std::filesystem;
#else
#include <sys/types.h>
#include <sys/stat.h>

#if defined(HAVE_STATX)
#include <fcntl.h>   // AT_* constants for statx
#endif

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "win32_path.h"
#endif

#endif


bool fs_equivalent(std::string_view path1, std::string_view path2)
{
  // non-existent paths are not equivalent

  std::error_code ec;

  auto handle_error = [&]() {
    fs_error_callback(path1, path2, ec);
    return false;
  };

#ifdef HAVE_CXX_FILESYSTEM

  if(bool e = Filesystem::equivalent(path1, path2, ec); !ec)
    return e;

#else

#if defined(_WIN32)
  // GetFileInformationByName has trouble with relative symlinks - it says they're not the same
  // thus we need to use GetFileInformationByHandle instead

  auto get_file_info = [&](std::string_view path, BY_HANDLE_FILE_INFORMATION& info) {
    HANDLE h = fs_win32_get_file_handle(path);
    if (h == INVALID_HANDLE_VALUE) {
      ec = std::error_code(static_cast<int>(GetLastError()), std::system_category());
      return false;
    }

    const BOOL ok = GetFileInformationByHandle(h, &info);
    const DWORD error = ok ? ERROR_SUCCESS : GetLastError();
    CloseHandle(h);

    if (!ok)
      ec = std::error_code(static_cast<int>(error), std::system_category());

    return ok != 0;
  };

  BY_HANDLE_FILE_INFORMATION f1{}, f2{};
  if (get_file_info(path1, f1) && get_file_info(path2, f2))
    return f1.dwVolumeSerialNumber == f2.dwVolumeSerialNumber &&
           f1.nFileIndexHigh == f2.nFileIndexHigh &&
           f1.nFileIndexLow == f2.nFileIndexLow;

#else

// https://www.man7.org/linux/man-pages/man7/inode.7.html
#if defined(HAVE_STATX)

  auto statx_call = [](std::string_view p, struct statx& x) {
    const std::string ps{p};
    return ::statx(AT_FDCWD, ps.c_str(), AT_NO_AUTOMOUNT, STATX_INO, &x) == 0;
  };

  if(struct statx x1, x2; statx_call(path1, x1) && statx_call(path2, x2))
    return x1.stx_dev_major == x2.stx_dev_major && x1.stx_dev_minor == x2.stx_dev_minor && x1.stx_ino == x2.stx_ino;
  else if (errno != ENOSYS)
    return handle_error();

#endif

  struct stat s1, s2;

  // https://www.boost.org/doc/libs/1_86_0/libs/filesystem/doc/reference.html#equivalent
  auto stat_call = [](std::string_view p, struct stat& s) {
    const std::string ps{p};
    return ::stat(ps.c_str(), &s) == 0;
  };

  if(stat_call(path1, s1) && stat_call(path2, s2))
    return s1.st_dev == s2.st_dev && s1.st_ino == s2.st_ino;

#endif

#endif // HAVE_CXX_FILESYSTEM

  return handle_error();
}
