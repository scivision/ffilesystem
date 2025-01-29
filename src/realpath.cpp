#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include "ffilesystem.h"

#ifndef _WIN32
#include <cstdlib> // for realpath
#endif

#include <string>
#include <string_view>


std::string fs_realpath(std::string_view path)
{
  // resolve real path
  // not well-defined for non-existing path--may return empty string.

#ifdef _WIN32
  return fs_win32_final_path(path);
#else
  std::string r(fs_get_max_path(), '\0');

  return realpath(path.data(), r.data())
    ? fs_trim(r)
    : std::string();
#endif

}
