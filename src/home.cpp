#include <string>
#include <string_view>

#include <system_error>

#if __has_include(<format>) && defined(_WIN32)
#include <format>  // IWYU pragma: keep
#endif


// get_profile_dir
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <UserEnv.h> // GetUserProfileDirectoryA
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

  if(struct passwd *pw = getpwuid(eff_uid); pw) FFS_LIKELY
    return pw;

  fs_print_error(
#ifdef __cpp_lib_format  // C++20
    std::format("uid: {}", eff_uid)
#else
    ""
#endif
    , "getpwuid");
#endif

  return {};
}


std::string fs_get_homedir()
{
  // has no trailing slash
  if(std::string home = fs_getenv(fs_is_windows() ? "USERPROFILE" : "HOME");
      !home.empty())  FFS_LIKELY
    return fs_drop_slash(home);

  return fs_get_profile_dir();
}


std::string fs_get_profile_dir()
{
  // has no trailing slash

  std::error_code ec;

  #if defined(_WIN32)
  // https://learn.microsoft.com/en-us/windows/win32/api/userenv/nf-userenv-getuserprofiledirectorya
  std::string path(fs_get_max_path(), '\0');
  // works on MSYS2, MSVC, oneAPI
  HANDLE h = nullptr;
  auto N = static_cast<DWORD>(path.size());

  const bool ok = OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &h) != 0 &&
    GetUserProfileDirectoryA(h, path.data(), &N);

  if(!CloseHandle(h))
    ec = std::make_error_code(std::errc::io_error);

  if (ok && !ec){
    path.resize(N-1);
    return fs_drop_slash(path);
  }
#else
  const struct passwd *pw = fs_getpwuid();
  if (pw)
    return fs_drop_slash(pw->pw_dir);
#endif

  fs_print_error("", "get_profile_dir", ec);
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
#if defined(_WIN32)
  // https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getusernamea
  std::string name(fs_get_max_path(), '\0');
  DWORD L = static_cast<DWORD>(name.size());
  // Windows.h
  if(GetUserNameA(name.data(), &L) != 0){
    name.resize(L-1);
    return name;
  }
#else
  const struct passwd *pw = fs_getpwuid();
  if (pw)
    return pw->pw_name;
#endif

  fs_print_error("", "get_username");
  return {};
}
