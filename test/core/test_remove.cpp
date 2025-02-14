#include <iostream>
#include <cstdlib>
#include <string>
#include <string_view>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main()
{

  const std::string j = "test_remove.txt";

  if(!fs_touch(j))
    err("touch failed for " + j);
  if(!fs_is_file(j))
    err(j + " is not a file");

  if(!fs_remove(j)){
    if(fs_exists(j))
      err("remove(" + j + ") failed--file still exists");
    err("remove(" + j + ") failed");
  }

  ok_msg("remove(" + j + ") C++");

  return EXIT_SUCCESS;
}
