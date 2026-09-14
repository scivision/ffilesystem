#if defined(__linux__) || defined(__CYGWIN__)
#if !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif
#endif

#include "ffilesystem.h"

#if defined(FFS_DARWIN)
#include <sys/syslimits.h>
#elif defined(_WIN32)
#include <cstdlib> // for _MAX_PATH
#endif

#if __has_include(<limits.h>)
#include <limits.h>
#endif


std::size_t
fs_get_max_path()
{
  // inspired by Boost::filesystem
  constexpr std::size_t default_max_path = 1024u;
  constexpr std::size_t absolute_max_path = 32u * default_max_path;

  // Returns the maximum path length supported by the file system.
  auto m = default_max_path;
#if defined(PATH_MAX)
  // POSIX
  m = PATH_MAX;
#elif defined(_MAX_PATH)
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/path-field-limits
  m = _MAX_PATH;
#elif defined(_POSIX_PATH_MAX)
  m = _POSIX_PATH_MAX;
#endif

  return (m < absolute_max_path) ? m : absolute_max_path;
}
