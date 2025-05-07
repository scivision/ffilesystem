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

#if __has_include(<source_location>)
#include <source_location>
#endif

#if __has_include(<format>)
#include <format>
#endif

#if defined(__cpp_lib_source_location) && defined(__cpp_lib_format)
#define FFS_SFMT
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


void fs_print_error(std::string_view path, std::string_view fname
#if defined(FFS_SFMT)
, const std::source_location& location){
  std::string s = std::format("{}:{}", location.file_name(), location.line());
#else
 ){ std::string s;
#endif

  std::cerr << "ERROR: Ffilesystem:" << fname << "(" << path << ")  " << s;

  fs_emit_error();

  std::cerr << std::endl;
}


void fs_print_error(std::string_view path, std::string_view fname, const std::error_code& ec
#if defined(FFS_SFMT)
, const std::source_location& location){
  std::string s = std::format("{}:{}", location.file_name(), location.line());
#else
 ){ std::string s;
#endif

  std::cerr << "ERROR: Ffilesystem:" << fname << "(" << path << ")  " << s;
  if(ec)
    std::cerr << ec.message() << " " << ec.value() << " ";

  fs_emit_error();

  std::cerr << std::endl;
}


void fs_print_error(std::string_view path1, std::string_view path2, std::string_view fname
#if defined(FFS_SFMT)
, const std::source_location& location){
  std::string s = std::format("{}:{}", location.file_name(), location.line());
#else
 ){ std::string s;
#endif

  std::cerr << "ERROR: Ffilesystem:" << fname << "(" << path1 <<  ", " << path2 << ")  " << s;

  fs_emit_error();

  std::cerr << std::endl;
}

void fs_print_error(std::string_view path1, std::string_view path2, std::string_view fname, const std::error_code& ec
#if defined(FFS_SFMT)
, const std::source_location& location){
  std::string s = std::format("{}:{}", location.file_name(), location.line());
#else
 ){
 std::string s;
#endif

  std::cerr << "ERROR: Ffilesystem:" << fname << "(" << path1 <<  ", " << path2 << ")  " << s;

  if(ec)
    std::cerr << "C++ exception:" << ec.message() <<  " " << ec.value() << " ";

  fs_emit_error();

  std::cerr << std::endl;
}
