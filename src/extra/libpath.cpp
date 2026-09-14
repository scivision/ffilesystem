#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // GetModuleHandleExW, GetModuleFileNameW
#elif defined(ffilesystem_HAVE_DLADDR)
#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif
#include <dlfcn.h> // dladdr
#endif

#include <string>

#include "ffilesystem.h"


#if defined(_WIN32) || defined(__CYGWIN__) || defined(ffilesystem_HAVE_DLADDR)
namespace {
void module_anchor() {}
}
#endif


std::string fs_lib_path()
{

#if defined(_WIN32) || defined(__CYGWIN__)
  HMODULE module = nullptr;
  if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                         GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                         reinterpret_cast<LPCWSTR>(&module_anchor), &module)) {
    std::wstring path(fs_get_max_path(), L'\0');
    const DWORD length = GetModuleFileNameW(module, path.data(),
                                            static_cast<DWORD>(path.size()));
    if (length > 0 && length < path.size())
      return fs_win32_to_narrow(std::wstring_view(path.data(), length));
  }
#elif defined(ffilesystem_HAVE_DLADDR)
  if(Dl_info info; dladdr(reinterpret_cast<void*>(&module_anchor), &info))
    return info.dli_fname;
#endif

  fs_error_callback("");
  return {};
}
