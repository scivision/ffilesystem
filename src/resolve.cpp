#include "ffilesystem.h"

#include <iostream>  // IWYU pragma: keep

#include <string>
#include <string_view>
#include <system_error>

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
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

  const std::string ex = expand_tilde
    ? fs_expanduser(path)
    : std::string(path);

  std::error_code ec;
  
#ifdef HAVE_CXX_FILESYSTEM

  if (fs_is_mingw() && fs_is_symlink(ex))
    return fs_win32_final_path(ex);

  const auto c = strict
    ? std::filesystem::canonical(ex, ec)
    : std::filesystem::weakly_canonical(ex, ec);

  if(!ec) FFS_LIKELY
    return fs_drop_slash(c.generic_string());
  
#else

  if (std::string c = fs_realpath(ex); !c.empty())
    return c;

  if (!strict)
    return fs_normal(ex);

#endif

  fs_print_error(path, "canonical", ec);
  return {};
}


std::string fs_resolve(std::string_view path, const bool strict, const bool expand_tilde)
{
  // works like canonical(absolute(path)).
  // Inspired by Python pathlib.Path.resolve()
  // https://docs.python.org/3/library/pathlib.html#pathlib.Path.resolve

  return fs_canonical(fs_absolute(path, expand_tilde), strict, false);
}
