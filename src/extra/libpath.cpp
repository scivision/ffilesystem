// must be before ANY other include
#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // GetModuleFileNameA
#elif defined(HAVE_DLADDR)
#include <dlfcn.h> // dladdr
static void dl_dummy_func() {}
#endif

#include <string>

#include "ffilesystem.h"


std::string fs_lib_path()
{

#if (defined(_WIN32) || defined(__CYGWIN__)) && defined(FS_DLL_NAME)
 // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea

  std::string path(fs_get_max_path(), '\0');

  if(DWORD L = GetModuleFileNameA(GetModuleHandleA(FS_DLL_NAME), path.data(), static_cast<DWORD>(path.size()));
      L > 0 && GetLastError() != ERROR_INSUFFICIENT_BUFFER)  FFS_LIKELY
  {
    path.resize(L);
    return path;
  }
#elif defined(HAVE_DLADDR)
  if(Dl_info info; dladdr(reinterpret_cast<void*>(&dl_dummy_func), &info))  FFS_LIKELY
    return info.dli_fname;
#endif

  fs_print_error("", __func__);
  return {};
}
