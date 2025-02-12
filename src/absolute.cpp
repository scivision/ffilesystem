#include <string>
#include <string_view>

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#include <system_error>
#endif

#include "ffilesystem.h"


std::string fs_absolute(std::string_view path, const bool expand_tilde)
{
  // wraps std::filesystem::absolute(path).
  // path need not exist
  // Inspired by Python pathlib.Path.absolute()
  // https://docs.python.org/3/library/pathlib.html#pathlib.Path.absolute

  // Linux, MinGW can't handle empty paths
  if(path.empty())
    return fs_get_cwd();

  const std::string ex = expand_tilde
    ? fs_expanduser(path)
    : std::string(path);

  if (ex.empty()) FFS_UNLIKELY
    return {};

  if (fs_is_absolute(ex))
    return ex;

#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;

  const auto a = std::filesystem::absolute(ex, ec);

  if(!ec) FFS_LIKELY
    return a.generic_string();

  fs_print_error(path, __func__, ec);
  return {};
#else
  std::string a = fs_get_cwd();
  if(a.empty())
    return {};

  a.push_back('/');
  a.append(ex);

  // NOT normalized to be consistent with <filesystem>
  return a;
#endif
}



std::string fs_absolute(std::string_view path, std::string_view base, const bool expand_tilde)
{
  // rebase path on base.

  const std::string ex = expand_tilde
    ? fs_expanduser(path)
    : std::string(path);

  if (ex.empty() && !path.empty()) FFS_UNLIKELY
    return {};

  if(fs_is_absolute(ex))
    return ex;

  std::string b = fs_absolute(base, expand_tilde);
  if(b.empty())  FFS_UNLIKELY
    return {};

  if(!ex.empty()){
    if(b.back() != '/')
      b.push_back('/');

    // don't need join(). Keeps it like Python pathlib.Path.absolute()
    b.append(ex);
  }

  return b;
}
