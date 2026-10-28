#include "ffilesystem.h"

#include <limits>
#include <string>
#include <system_error>

# include <sys/types.h> // for pid_t


namespace ffilesystem {

bool is_rosetta(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return false;
}

bool is_wsl(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return false;
}

std::string cpu_arch(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return {};
}

std::string get_profile_dir()
{
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return {};
}

std::string get_shell(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return {};
}

std::string get_terminal(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return {};
}

std::string get_username()
{
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return {};
}

std::string hostname(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return {};
}

std::string os_version(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return {};
}

unsigned long long total_sys_memory()
{
  fs_error_callback("fs_total_sys_memory: system support disabled",
                    std::make_error_code(std::errc::function_not_supported));
  return 0;
}

unsigned long long get_free_memory()
{
  fs_error_callback("fs_get_free_memory: system support disabled",
                    std::make_error_code(std::errc::function_not_supported));
  return std::numeric_limits<unsigned long long>::max();
}

// uid.cpp
bool is_admin(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return false;
}

pid_t getpid(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return -1;
}

bool stdin_tty(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return false;
}

} // namespace ffilesystem
