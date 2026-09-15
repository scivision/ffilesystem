#if defined(__linux__)
#if !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif
#if defined(HAVE_STATX)
#include <fcntl.h>   // AT_* constants for statx
#endif
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <string_view>
#include <string>

#include "ffilesystem.h"
#include "internal.h"

mode_t
fs_st_mode(std::string_view path)
{

  const std::string cpath{path};
#if defined(HAVE_STATX)
// Linux Glibc only
// https://www.gnu.org/software/gnulib/manual/html_node/statx.html
// https://www.man7.org/linux/man-pages/man2/statx.2.html

  if (struct statx x; ::statx(AT_FDCWD, cpath.c_str(), AT_NO_AUTOMOUNT, STATX_MODE, &x) == 0) {
    return x.stx_mode;
  } else if (errno != ENOSYS) {
    return 0;
  }
#endif

// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/stat-functions
  if (struct stat s; ::stat(cpath.c_str(), &s) == 0)
    return s.st_mode;

  return 0;
}
