#include <string>
#include <string_view>

#include <system_error>

#if __has_include(<format>)
#include <format>  // IWYU pragma: keep
#endif


// get_profile_dir
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef SECURITY_WIN32
#define SECURITY_WIN32
#endif
#include <UserEnv.h> // GetUserProfileDirectoryA
#include <Security.h> // GetUserNameExA
#include <Windows.h>
#else
#include <sys/types.h>  // IWYU pragma: keep
#include <pwd.h>      // for getpwuid, passwd
#include <unistd.h> // for mac too
#endif

#include "ffilesystem.h"


struct passwd* fs_getpwuid()
{
#if !defined(_WIN32)
  const uid_t eff_uid = geteuid();

  if(auto pw = getpwuid(eff_uid)) FFS_LIKELY
    return pw;

  fs_print_error(
#if defined(__cpp_lib_format)  // C++20
    std::format("uid: {}", eff_uid)
#else
    ""
#endif
    , __func__);
#endif

  return {};
}


std::string fs_get_homedir()
{
  // has no trailing slash
  std::string home = fs_getenv(fs_is_windows() ? "USERPROFILE" : "HOME");

  return home.empty() ? fs_get_profile_dir() : fs_drop_slash(home);
}


std::string fs_get_profile_dir()
{
  // has no trailing slash

#if defined(_WIN32)
  // https://learn.microsoft.com/en-us/windows/win32/api/userenv/nf-userenv-getuserprofiledirectorya
  // works on MSYS2, MSVC, oneAPI
  HANDLE h = nullptr;

  if(OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &h)) {
    DWORD L = 0;
    GetUserProfileDirectoryW(h, nullptr, &L);
    BOOL ok = false;
    std::wstring w;

    if(L > 0) {
      w.resize(L);
      ok = GetUserProfileDirectoryW(h, w.data(), &L);
    }

    if(CloseHandle(h) && ok && L > 0)
      return fs_drop_slash(fs_win32_to_narrow(w));
  }
#else
  if (auto pw = fs_getpwuid())
    return fs_drop_slash(pw->pw_dir);
#endif

  fs_print_error("", __func__);
  return {};
}


std::string fs_expanduser(std::string_view path)
{
  if(path.empty()) FFS_UNLIKELY
    return {};

  if(path.front() != '~')
    return std::string(path);

  // second character is not a file separator
  // std::set is much slower than a simple if
  if(path.length() > 1 && !(path[1] == '/' || (fs_is_windows() && path[1] == '\\')))
    return std::string(path);

  std::string home = fs_get_homedir();
  if(home.empty()) FFS_UNLIKELY
    return {};

  if (path.length() < 3)
    return home;

// handle initial duplicated file separators. NOT .lexical_normal to handle "~/.."
// std::set is much slower than a simple if
  std::string::size_type i = 2;
  while(i < path.length() && (path[i] == '/' || (fs_is_windows() && path[i] == '\\')))
    i++;

  std::string e = home + "/" + std::string(path).substr(i);

  if (e.back() == '/')
    e.pop_back();

  return e;
}


std::string fs_get_username()
{
  // Get username of the current user

#if defined(_WIN32)

// https://learn.microsoft.com/en-us/windows/win32/api/secext/nf-secext-getusernameexa
// https://learn.microsoft.com/en-us/windows/win32/api/secext/ne-secext-extended_name_format
  ULONG L = 0;
  if (GetUserNameExW(NameSamCompatible, nullptr, &L) == 0 && L > 0) {
    std::wstring w;
    w.resize(L);
    if (GetUserNameExW(NameSamCompatible, w.data(), &L) != 0)
      return fs_win32_to_narrow(w);
  }

#else

  if (auto pw = fs_getpwuid())
    return pw->pw_name;

#endif

  fs_print_error("", __func__);
  return {};
}
