#include "ffilesystem.h"

#include <string_view>
#include <string>


namespace {
struct fs_remove_on_exit {
  std::string path;

  ~fs_remove_on_exit()
  {
    if(!path.empty())
      ffilesystem::remove(path);
  }
};

}


namespace ffilesystem {

bool is_case_sensitive(std::string_view path)
{
  // check if the filesystem is case sensitive by creating a temporary file
  // with a mixed-case name and checking if the same name with different case
  // resolves to the same file.

  if(path.empty() || !is_directory(path))
    return false;

  constexpr std::string::size_type N = 16; // arbitrary

  const std::string rname = generate_random_alphanumeric_string(N);
  if(rname.empty())
    return false;

  const std::string mixed_name = ".FfilesystemCaseSensitiveCheck_" + rname;
  const std::string mixed_path = join(path, mixed_name);

  if(!touch(mixed_path))
    return false;

  const fs_remove_on_exit cleanup{mixed_path};

  std::string lower_name = mixed_name;
  ascii_lower(lower_name);

  const std::string lower_path = join(path, lower_name);

  // Case-insensitive filesystems resolve both names to the same file.
  return equivalent(mixed_path, lower_path);
}

}
