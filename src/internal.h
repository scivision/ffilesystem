#pragma once

#include "ffilesystem.h"

#include <cerrno>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(HAVE_STATX)
#include <fcntl.h>
#endif

#include <string_view>

namespace fs_internal {

inline void report_stat_error(std::string_view path)
{
  fs_error_callback(path);
}

inline void ignore_stat_error(std::string_view)
{}

#if defined(HAVE_STATX)
template <typename T, typename ErrorHandler, typename StatxValue, typename StatValue>
T get_stat_value(std::string_view path, T error_value, ErrorHandler error_handler,
                 unsigned int statx_mask,
                 StatxValue statx_value, StatValue stat_value)
#else
template <typename T, typename ErrorHandler, typename StatValue>
T get_stat_value(std::string_view path, T error_value, ErrorHandler error_handler,
                 StatValue stat_value)
#endif
{
  const std::string cpath{path};

#if defined(HAVE_STATX)
  if (struct statx x; ::statx(AT_FDCWD, cpath.c_str(), AT_NO_AUTOMOUNT, statx_mask, &x) == 0)
    return statx_value(x);
  else if (errno != ENOSYS) {
    error_handler(path);
    return error_value;
  }
#endif

  if (struct stat s; ::stat(cpath.c_str(), &s) == 0)
    return stat_value(s);

  error_handler(path);
  return error_value;
}

} // namespace fs_internal

#ifdef _MSC_VER
#ifndef mode_t
using mode_t = unsigned int;
#endif
#endif

mode_t fs_st_mode(std::string_view);
