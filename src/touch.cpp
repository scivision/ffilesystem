#include <string_view>
#include <fstream> // std::ofstream

#include "ffilesystem.h"


bool fs_touch(std::string_view path)
{
  if(fs_set_modtime(path, true))
    return true;

  if(std::ofstream f(path.data()); f){
    f.close();
    return true;
  }

  fs_print_error(path, __func__);
  return false;
}
