#include "ffilesystem.h"

#include <string_view>
#include <string>

#include <algorithm> // std::transform
#include <cctype> // std::tolower
#include <iterator> // std::back_inserter

bool fs_is_case_sensitive(std::string_view path)
{
  // check if the filesystem is case sensitive
  if(path.empty())
    return false;

  const std::string rname = fs_generate_random_alphanumeric_string(16);
  std::string p(path);
  // create a temporary file with a unique name that has CamelCase
  const std::string CamelCase = p + "/.FfilesystemCaseSensitiveCheck_" + rname;

  if(!fs_touch(CamelCase))
    return false;

  std::string lower;
  std::transform(CamelCase.begin(), CamelCase.end(), std::back_inserter(lower), ::tolower);

  const std::string actual = fs_canonical(lower, true);

  if(!rname.empty())
    fs_remove(CamelCase);

  return actual == CamelCase;

}
