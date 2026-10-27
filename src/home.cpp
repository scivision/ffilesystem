#include <string>
#include <string_view>

#include "ffilesystem.h"



std::string fs_get_homedir()
{
  if (auto h = fs_getenv(ffilesystem::is_windows ? "USERPROFILE" : "HOME"); h && !h->empty())
    return *h;

  return fs_get_profile_dir();
}


std::string fs_expanduser(std::string_view path)
{
  if(path.empty())
    return {};

  if(path.front() != '~')
    return std::string(path);

  // second character is not a file separator
  if(path.length() > 1 && !ffilesystem::is_file_separator(path[1]))
    return std::string(path);

  std::string e = fs_get_homedir();
  if(e.empty() || path.length() < 3)
    return e;

  if (!ffilesystem::is_file_separator(e.back()))
    e.push_back('/');

  // handle initial duplicated file separators. NOT .lexical_normal to handle "~/.."
  if (auto i = path.find_first_not_of(ffilesystem::file_separators, 2); i != std::string_view::npos)
    e.append(path.substr(i));

  return e;
}
