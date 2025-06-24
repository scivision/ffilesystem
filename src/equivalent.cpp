#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include "ffilesystem.h"

#include <string_view>
#include <system_error>
#include <iostream>  // IWYU pragma: keep

#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Filesystem = std::filesystem;
#else
#include <sys/types.h>
#include <sys/stat.h>

#if defined(__linux__) && defined(USE_STATX)
#include <fcntl.h>   // AT_* constants for statx()
#endif

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#endif


bool fs_equivalent(std::string_view path1, std::string_view path2)
{
  // non-existent paths are not equivalent

  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM

  std::string p1(path1);
  std::string p2(path2);

  if(bool e = Filesystem::equivalent(p1, p2, ec); !ec)
    return e;

#else

#if defined(_WIN32)
// FUTURE: GetFileInformationByName Windows ~24H2
// https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getfileinformationbyname
// https://github.com/rust-lang/rust/issues/130169
//
// for now use GetFileInformationByHandle
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileinformationbyhandle#remarks
// FILE_FLAG_BACKUP_SEMANTICS to allow opening directories

  HANDLE h1 = CreateFileW(fs_win32_to_wide(path1).data(), FILE_READ_ATTRIBUTES, FILE_SHARE_READ, nullptr,
    OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
  if(h1 == INVALID_HANDLE_VALUE) {
    fs_print_error(path1, __func__);
    return false;
  }

  HANDLE h2 = CreateFileW(fs_win32_to_wide(path2).data(), FILE_READ_ATTRIBUTES, FILE_SHARE_READ, nullptr,
    OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
  if(h2 == INVALID_HANDLE_VALUE) {
    fs_print_error(path2, __func__);
    CloseHandle(h1);
    return false;
  }

  BY_HANDLE_FILE_INFORMATION f1, f2;
  // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/ns-fileapi-by_handle_file_information
  BOOL ok1 = GetFileInformationByHandle(h1, &f1);
  BOOL ok2 = GetFileInformationByHandle(h2, &f2);
  CloseHandle(h1);
  CloseHandle(h2);
  if(ok1 && ok2) {
    return f1.dwVolumeSerialNumber == f2.dwVolumeSerialNumber &&
           f1.nFileIndexHigh == f2.nFileIndexHigh &&
           f1.nFileIndexLow == f2.nFileIndexLow;
  }

#else
  int r1 = 0;
  int r2 = 0;

// https://www.man7.org/linux/man-pages/man7/inode.7.html
#if defined(STATX_INO) && defined(USE_STATX)

  struct statx x1;
  struct statx x2;

  r1 = statx(AT_FDCWD, path1.data(), AT_NO_AUTOMOUNT, STATX_INO, &x1);
  if(r1 == 0){
    r2 = statx(AT_FDCWD, path2.data(), AT_NO_AUTOMOUNT, STATX_INO, &x2);
    if(r2 == 0)
      return x1.stx_dev_major == x2.stx_dev_major && x1.stx_dev_minor == x2.stx_dev_minor && x1.stx_ino == x2.stx_ino;
  }

#endif

  if((r1 == 0 && r2 == 0) || errno == ENOSYS){
    struct stat s1;
    struct stat s2;

    // https://www.boost.org/doc/libs/1_86_0/libs/filesystem/doc/reference.html#equivalent
    if(!stat(path1.data(), &s1) && !stat(path2.data(), &s2))
      return s1.st_dev == s2.st_dev && s1.st_ino == s2.st_ino;
  }

#endif
#endif

  fs_print_error(path1, path2, __func__, ec);
  return false;
}
