#include <string>
#include <string_view>

#include <locale>

#include "ffilesystem.h"

#if defined(ffilesystem_locale)

#if __has_include(<codecvt>)
#define ffilesystem_HAVE_CODECVT
#include <codecvt>
#endif

#endif


std::string fs_get_locale_name() {
// https://en.cppreference.com/w/cpp/locale/locale/name
    return std::locale("").name();
}


std::string fs_to_narrow(std::wstring_view w)
{
  if(w.empty())
    return {};

  // https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
#if defined(ffilesystem_HAVE_CODECVT)
  // deprecated in C++17, but no STL replacement
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.to_bytes(w.data());
#else
  fs_print_error("", __func__, std::make_error_code(std::errc::function_not_supported));
  return {};
#endif

}

std::wstring fs_to_wide(std::string_view n)
{
  if(n.empty())
    return {};

#if defined(ffilesystem_HAVE_CODECVT)
  // deprecated in C++17, but no STL replacement
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.from_bytes(n.data());
#else
  fs_print_error("", __func__, std::make_error_code(std::errc::function_not_supported));
  return {};
#endif
}
