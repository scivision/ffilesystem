// IWYU pragma: no_include <bits/statx-generic.h>
// IWYU pragma: no_include <linux/stat.h>

#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <aclapi.h> // GetNamedSecurityInfo
#include <Windows.h>
#else
#include <sys/types.h>  // IWYU pragma: keep
#include <sys/stat.h>  // IWYU pragma: keep
#include <pwd.h>      // for getpwuid
#include <grp.h>     // for getgrgid
#include <optional>
#endif

#if defined(__linux__) && defined(USE_STATX)
#include <iostream>
#include <fcntl.h>   // AT_* constants for statx()
#endif

#include <string>
#include <string_view>

#include "ffilesystem.h"


#if defined(_WIN32)
static std::string fs_win32_get_owner(PSID pSid)
{
  DWORD L1 = 0;
  DWORD L2 = 0;
  SID_NAME_USE eUse = SidTypeUnknown;

  if(!LookupAccountSidA(nullptr, pSid, nullptr, &L1, nullptr, &L2, &eUse) && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    return {};

  std::string s(L1, '\0');
  if (!LookupAccountSidA(nullptr, pSid, s.data(), &L1, nullptr, &L2, &eUse))
    return {};

  // it's L1, not L1 - 1
  s.resize(L1);
  return s;
}
#else

static std::optional<gid_t> fs_stat_uid(std::string_view path)
{

  int r = 0;

#if defined(STATX_UID) && defined(USE_STATX)
  // https://www.man7.org/linux/man-pages/man2/statx.2.html
  if (fs_trace) std::cout << "TRACE: statx() owner_name " << path << "\n";

  struct statx sx;
  r = statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_UID, &sx);
  if (r == 0)
    return sx.stx_uid;
#endif

  if(r == 0 || errno == ENOSYS){
    struct stat s;
    if(!stat(path.data(), &s))
      return s.st_uid;
  }

  return {};
}

static std::optional<gid_t> fs_stat_gid(std::string_view path)
{
  int r = 0;
#if defined(STATX_GID) && defined(USE_STATX)
  if (fs_trace) std::cout << "TRACE: statx() owner_group " << path << "\n";

  struct statx sx;
  r = statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_GID, &sx);
  if (r == 0)
    return sx.stx_gid;
#endif

if(r == 0 || errno == ENOSYS){
    struct stat s;
    if(!stat(path.data(), &s))
      return s.st_gid;
  }

  return {};
}

#endif


std::string
fs_get_owner_name(std::string_view path)
{
#if defined(_WIN32)
// https://learn.microsoft.com/en-us/windows/win32/secauthz/finding-the-owner-of-a-file-object-in-c--

  PSECURITY_DESCRIPTOR pSD = nullptr;
  PSID pSid = nullptr;
  std::string s;

  // https://learn.microsoft.com/en-us/windows/win32/api/aclapi/nf-aclapi-getnamedsecurityinfoa
  if(GetNamedSecurityInfoA(path.data(), SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, &pSid, nullptr, nullptr, nullptr, &pSD) == ERROR_SUCCESS)
    s = fs_win32_get_owner(pSid);

  LocalFree(pSD);
  // https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-localfree
  if(!s.empty())
    return s;

#else
  if (auto uid = fs_stat_uid(path)) {
    if (auto pw = getpwuid(uid.value()))
      return pw->pw_name;
  }
#endif

  fs_print_error(path, "get_owner_name");
  return {};
}


std::string
fs_get_owner_group(std::string_view path)
{
#if defined(_WIN32)
  // use GetNamedSecurityInfoA to get group name

  PSECURITY_DESCRIPTOR pSD = nullptr;
  PSID pSid = nullptr;
  std::string s;

  // https://learn.microsoft.com/en-us/windows/win32/api/aclapi/nf-aclapi-getnamedsecurityinfoa
  if(GetNamedSecurityInfoA(path.data(), SE_FILE_OBJECT, GROUP_SECURITY_INFORMATION, nullptr, &pSid, nullptr, nullptr, &pSD) == ERROR_SUCCESS)
    s = fs_win32_get_owner(pSid);

  LocalFree(pSD);
  if(!s.empty())
    return s;

#else
  if (auto gid = fs_stat_gid(path)) {
    if (auto gr = getgrgid(gid.value()))
      return gr->gr_name;
  }
#endif

  fs_print_error(path, "get_owner_group");
  return {};
}
