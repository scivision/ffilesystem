#include <string>

// get_profile_dir
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#ifndef SECURITY_WIN32
#define SECURITY_WIN32
#endif
#include "win32_path.h"
#include <UserEnv.h> // GetUserProfileDirectory
#include <Security.h> // GetUserNameEx
#include <Windows.h>
#else
#include <sys/types.h>  // IWYU pragma: keep
#include <pwd.h>      // for getpwuid, passwd
#include <unistd.h> // for mac too
#endif

#include "ffilesystem.h"
#include "sys/internal_sys.h"


#if !defined(_WIN32)
struct passwd* fs_getpwuid()
{
  const uid_t eff_uid = ::geteuid();

  if(auto pw = ::getpwuid(eff_uid))
    return pw;

  fs_error_callback(std::to_string(eff_uid));
  return {};
}
#endif


std::string fs_get_profile_dir()
{

#if defined(_WIN32)
  // https://learn.microsoft.com/en-us/windows/win32/api/userenv/nf-userenv-getuserprofiledirectorya
  // works on MSYS2, MSVC, oneAPI
  HANDLE h = nullptr;

  if(OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &h)) {
    DWORD L = 0;
    GetUserProfileDirectoryW(h, nullptr, &L);
    if (L <= 0){
      fs_error_callback("GetUserProfileDirectoryW");
      return {};
    }

    std::wstring w(L, '\0');
    DWORD Lr{L};
    BOOL const ok = GetUserProfileDirectoryW(h, w.data(), &Lr);
    CloseHandle(h);

    if(ok && Lr == L)
      return fs_win32_to_narrow(w);
  }
#else
  if (auto pw = fs_getpwuid())
    return pw->pw_dir;
#endif

  fs_error_callback("");
  return {};
}


std::string fs_get_username()
{
  // Get username of the current user

#if defined(_WIN32)

// https://learn.microsoft.com/en-us/windows/win32/api/secext/nf-secext-getusernameexa
// https://learn.microsoft.com/en-us/windows/win32/api/secext/ne-secext-extended_name_format
  ULONG L = 0;
  if (GetUserNameExW(NameSamCompatible, nullptr, &L) == 0 && L > 0) {
    std::wstring w(L, L'\0');
    if (GetUserNameExW(NameSamCompatible, w.data(), &L) != 0)
      return fs_win32_to_narrow(w);
  }

#else

  if (auto pw = fs_getpwuid())
    return pw->pw_name;

#endif

  fs_error_callback("");
  return {};
}
