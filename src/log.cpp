#include <system_error>

#include <iostream>
#include <ostream> // for std::endl

#include <string>
#include <string_view>

#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <cerrno>
#endif


static void fs_emit_error()
{
#if defined(_WIN32) || defined(__CYGWIN__)
  if (DWORD error = GetLastError(); error)
    std::cerr << "GetLastError:" << std::system_category().message(error) << " ";
#else
  if(errno){
    auto econd = std::generic_category().default_error_condition(errno);
    std::cerr << "errno:" << econd.message() << " ";
  }
#endif
}


void fs_print_error(std::string_view path, std::string_view fname)
{

  std::cerr << "ERROR: Ffilesystem:" << fname << "(" << path << ")  ";

  fs_emit_error();

  std::cerr << std::endl;
}


void fs_print_error(std::string_view path, std::string_view fname, const std::error_code& ec)
{
  std::cerr << "ERROR: Ffilesystem:" << fname << "(" << path << ")  ";
  if(ec)
    std::cerr << ec.message();

  fs_emit_error();

  std::cerr << std::endl;
}


void fs_print_error(std::string_view path1, std::string_view path2, std::string_view fname)
{
  std::cerr << "ERROR: Ffilesystem:" << fname << "(" << path1 <<  ", " << path2 << ")  ";

  fs_emit_error();

  std::cerr << std::endl;
}

void fs_print_error(std::string_view path1, std::string_view path2, std::string_view fname, const std::error_code& ec)
{
  std::cerr << "ERROR: Ffilesystem:" << fname << "(" << path1 <<  ", " << path2 << ")  ";

  if(ec)
    std::cerr << "C++ exception:" << ec.message() <<  " " << ec.value() << " ";

  fs_emit_error();

  std::cerr << std::endl;
}
