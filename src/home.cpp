#include <string>
#include <string_view>

#include "ffilesystem.h"



std::string fs_get_homedir()
{
  if (auto h = fs_getenv(fs_is_windows() ? "USERPROFILE" : "HOME"); h && !h->empty())
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
  // std::set is much slower than a simple if
  if(path.length() > 1 && !(path[1] == '/' || path[1] == fs_filesep()))
    return std::string(path);

  std::string e = fs_get_homedir();
  if(e.empty() || path.length() < 3)
    return e;

// handle initial duplicated file separators. NOT .lexical_normal to handle "~/.."
// std::set is much slower than a simple if
  std::string::size_type i = 2;
  while(i < path.length() && (path[i] == '/' || path[i] == fs_filesep()))
    i++;

  if (e.back() != '/' && e.back() != fs_filesep())
    e.push_back('/');

  return e.append(path.substr(i));
}
