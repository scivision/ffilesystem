#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include <cstdlib> // putenv, setenv, getenv

#include <string>
#include <string_view>

#if defined(_WIN32)
#include <algorithm> // std::replace
#include <objbase.h> // IWYU pragma: keep
// CoTaskMemFree
#include <KnownFolders.h> // FOLDERID_LocalAppData
#include <shlobj.h> // SHGetKnownFolderPath
#endif

#include "ffilesystem.h"


std::string fs_getenv(std::string_view name)
{
  // convenience function to get environment variable without needing to check for nullptr
  // don't emit error because sometimes we just check if envvar is defined

  auto buf = std::getenv(name.data());

  return buf ? std::string(buf) : std::string();
}


bool fs_setenv(std::string_view name, std::string_view value)
{

#if defined(_WIN32)
  // SetEnvironmentVariable returned OK but set blank values
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/putenv-wputenv

  if(_putenv_s(name.data(), value.data()) == 0)  FFS_LIKELY
    return true;
#else
  // https://www.man7.org/linux/man-pages/man3/setenv.3.html
  if(setenv(name.data(), value.data(), 1) == 0)  FFS_LIKELY
    return true;
#endif

  fs_print_error(name, __func__);
  return false;
}


std::string fs_user_config_dir()
{
#if defined(_WIN32)
  PWSTR s = nullptr;
  std::string r;
  if(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &s) == S_OK)  FFS_LIKELY
    r = fs_win32_to_narrow(s);

  CoTaskMemFree(s);
  // https://learn.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-cotaskmemfree

  if (!r.empty())  FFS_LIKELY
    return r;
#else
  if(std::string h = fs_getenv("XDG_CONFIG_HOME"); !h.empty())
    return h;
  if(std::string h = fs_getenv("HOME"); !h.empty())
    return h + "/.config";
#endif

  fs_print_error("", __func__);
  return {};
}
