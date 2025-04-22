#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include "ffilesystem.h"

#include <string_view>
#include <system_error>
#include <iostream>  // IWYU pragma: keep

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#else
#include <sys/types.h>
#include <sys/stat.h>

#if defined(__linux__) && defined(USE_STATX)
#include <fcntl.h>   // AT_* constants for statx()
#endif

#endif


bool fs_equivalent(std::string_view path1, std::string_view path2)
{
  // non-existent paths are not equivalent

  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM

  std::string p1(path1);
  std::string p2(path2);

  if(fs_is_mingw()){
    // MinGW falsely gives mismatch if not canonicalized
    p1 = fs_canonical(p1, true, false);
    p2 = fs_canonical(p2, true, false);
  }

  bool e = std::filesystem::equivalent(p1, p2, ec);
  if(!ec)
    return e;

#else

  int r1 = 0;
  int r2 = 0;

#if defined(STATX_BASIC_STATS) && defined(USE_STATX)

  struct statx x1;
  struct statx x2;

  r1 = statx(AT_FDCWD, path1.data(), AT_NO_AUTOMOUNT, STATX_BASIC_STATS, &x1);
  if(r1 == 0){
    r2 = statx(AT_FDCWD, path2.data(), AT_NO_AUTOMOUNT, STATX_BASIC_STATS, &x2);
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

  fs_print_error(path1, path2, __func__, ec);
  return false;
}
