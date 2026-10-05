#include "ffilesystem.h"

#include <limits>
#include <string>
#include <system_error>

# include <sys/types.h> // for pid_t

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

// uid.cpp
bool fs_is_admin(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return false;
}

pid_t fs_getpid(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return -1;
}

std::string fs_get_terminal(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return {};
}

bool fs_stdin_tty(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return false;
}

// user.cpp
std::string fs_get_profile_dir()
{
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return {};
}

std::string fs_get_username()
{
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return {};
}

// shell.cpp
std::string fs_get_shell(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return {};
}

// winsock.cpp
std::string fs_hostname(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return {};
}
