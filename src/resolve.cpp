#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE // for realpath
#endif

#include "ffilesystem.h"

#include <string>
#include <string_view>
#include <system_error>

#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Filesystem = std::filesystem;
#endif

#if !defined(_WIN32)
#include <cstdlib> // for realpath
#endif


std::string
fs_canonical(
  std::string_view path,
  const bool strict,
  const bool expand_tilde)
{
  // canonicalize path, i.e. resolve all symbolic links, remove ".", ".." and extra slashes
  // if strict is true, then path must exist

  if (path.empty())
    return {};
    // need this for macOS otherwise it returns the current working directory instead of empty string

  std::string ex = expand_tilde
    ? fs_expanduser(path)
    : std::string(path);

  std::error_code ec;

#if defined(HAVE_CXX_FILESYSTEM)

  if (fs_is_mingw() && fs_is_symlink(ex))
    return fs_win32_final_path(ex);

  if(auto c = strict ? Filesystem::canonical(ex, ec) : Filesystem::weakly_canonical(ex, ec); !ec)
    return c.generic_string();

#else

  if (std::string c = fs_realpath(ex); !c.empty())
    return c;

  if (!strict)
    return fs_normal(ex);
  // std::filesystem::weakly_canonical() and boost::filesystem::canonical()
  // resolve the path to the current working directory for non-existing paths.
  // https://github.com/boostorg/filesystem/blob/f4bb6d0f3ebe9f8b90243d8a98989191925d49d2/src/operations.cpp#L5023
  // That has significant complexity we will not implement here, as this is just a fallback method.
  // We just return the normal path.

#endif

  fs_print_error(path, __func__, ec);
  return {};
}


std::string fs_resolve(std::string_view path, const bool strict, const bool expand_tilde)
{
  // works like canonical(absolute(path)).
  // Inspired by Python pathlib.Path.resolve()
  // https://docs.python.org/3/library/pathlib.html#pathlib.Path.resolve

  return fs_canonical(fs_absolute(path, expand_tilde), strict, false);
}


std::string fs_realpath(std::string_view path)
{
  // resolve real existing path
  // not defined for non-existing path--may return empty string.
  // https://man7.org/linux/man-pages/man3/realpath.3.html
  // https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/realpath.3.html

#if defined(_WIN32)
  return fs_win32_final_path(path);
#else
  if(char* r = realpath(path.data(), nullptr); r) {
    std::string result(r);
    free(r);
    return result;
  }
  return {};
#endif

}
