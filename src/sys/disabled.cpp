#include "ffilesystem.h"

#include <limits>
#include <string>
#include <system_error>

// uname.cpp
std::string fs_cpu_arch(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return {};
}

bool fs_is_wsl(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return false;
}

std::string fs_os_version(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return {};
}

// memory.cpp
unsigned long long fs_total_sys_memory()
{
  fs_error_callback("fs_total_sys_memory: system support disabled",
                    std::make_error_code(std::errc::function_not_supported));
  return 0;
}

unsigned long long fs_get_free_memory()
{
  fs_error_callback("fs_get_free_memory: system support disabled",
                    std::make_error_code(std::errc::function_not_supported));
  return std::numeric_limits<unsigned long long>::max();
}

// sysctl.cpp
bool fs_is_rosetta(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return false;
}
