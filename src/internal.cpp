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
#if defined(HAVE_STATX)
// Linux Glibc only
// https://www.gnu.org/software/gnulib/manual/html_node/statx.html
// https://www.man7.org/linux/man-pages/man2/statx.2.html

  return fs_internal::get_stat_value<mode_t>(
      path, {}, fs_internal::ignore_stat_error, STATX_MODE,
      [](const struct statx& x) { return static_cast<mode_t>(x.stx_mode); },
      [](const struct stat& s) { return s.st_mode; });
#else
  return fs_internal::get_stat_value<mode_t>(
      path, {}, fs_internal::ignore_stat_error,
      [](const struct stat& s) { return s.st_mode; });
#endif

}
