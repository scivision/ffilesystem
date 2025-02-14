#include <iostream>
#include <cstdlib>
#include <string>
#include <string_view>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main()
{

  const std::string j = "日本語_remove.txt";

  if(!fs_touch(j))
    err("touch failed for " + j);

  if(!fs_remove(j)){
    if(fs_exists(j))
      err("remove(" + j + ") failed--file still exists");
    err("remove(" + j + ") failed");
  }

  ok_msg("remove(" + j + ") C++");

  return EXIT_SUCCESS;
}
