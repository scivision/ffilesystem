#include "ffilesystem.h"

#include <string>
#include <string_view>

#include <system_error>

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#endif


std::string fs_relative_to(std::string_view base, std::string_view other)
{
  // find relative path from base to other
  // "base" and "other" are treated as weakly canonical paths

  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM
  if(std::string r = std::filesystem::relative(other, base, ec).generic_string(); !ec)
    return r;
  // this implements
  // std::filesystem::weakly_canonical(other, ec).lexically_relative(std::filesystem::weakly_canonical(base, ec)).generic_string();
#else
   ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_print_error(base, other, __func__, ec);
  return {};
}


std::string fs_proximate_to(std::string_view base, std::string_view other)
{
  // find proximate path from base to other
  // "base" and "other" are treated as weakly canonical paths

  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM
  if(std::string r = std::filesystem::proximate(other, base, ec).generic_string(); !ec)
    return r;
#else
   ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_print_error(base, other, __func__, ec);
  return {};
}
