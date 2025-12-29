#include <system_error>

#include <iostream>

#include <string>
#include <string_view>

#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <cerrno>
#endif


void fs_emit_error()
{
#if defined(_WIN32) || defined(__CYGWIN__)
  if (DWORD error = GetLastError(); error)
    std::cerr << "GetLastError: " << std::system_category().message(error) << " ";
#else
  if(errno){
    auto econd = std::generic_category().default_error_condition(errno);
    std::cerr << "errno: " << econd.message() << "\n";
  }
#endif
}


void fs_print_error(std::string_view path, std::string_view fname)
{

  std::cerr << "ERROR: Ffilesystem: " << fname << "(" << path << ")\n";

  fs_emit_error();
}


void fs_print_error(std::string_view path, std::string_view fname, const std::error_code& ec)
{
  std::cerr << "ERROR: Ffilesystem: " << fname << "(" << path << ")\n";
  if(ec)
    std::cerr << "C++ exception: " << ec.message() << " " << ec.value() << "\n";

  fs_emit_error();
}


void fs_print_error(std::string_view path1, std::string_view path2, std::string_view fname)
{
  std::cerr << "ERROR: Ffilesystem: " << fname << "(" << path1 <<  ", " << path2 << ")\n";

  fs_emit_error();
}

void fs_print_error(std::string_view path1, std::string_view path2, std::string_view fname, const std::error_code& ec)
{
  std::cerr << "ERROR: Ffilesystem: " << fname << "(" << path1 <<  ", " << path2 << ")\n";

  if(ec)
    std::cerr << "C++ exception: " << ec.message() <<  " " << ec.value() << "\n";

  fs_emit_error();
}
