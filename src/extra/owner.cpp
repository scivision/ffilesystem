// IWYU pragma: no_include <bits/statx-generic.h>
// IWYU pragma: no_include <linux/stat.h>

#if defined(__linux__) || defined(__CYGWIN__)
#if !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif
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

#include <cerrno>
#include <string>
#include <string_view>

#include "ffilesystem.h"
#include "internal.h"
#include "win32_path.h"


namespace {

#if defined(_WIN32)

std::string fs_win32_get_owner(const PSID pSid)
{
  DWORD L1{0};
  DWORD L2{0};
  SID_NAME_USE eUse{SidTypeUnknown};

  if(!LookupAccountSidW(nullptr, pSid, nullptr, &L1, nullptr, &L2, &eUse) && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    return {};

  std::wstring s(L1, L'\0');
  std::wstring domain(L2, L'\0');

  if (!LookupAccountSidW(nullptr, pSid, s.data(), &L1, domain.data(), &L2, &eUse))
    return {};

  return fs_win32_to_narrow(s);
}

std::string fs_win32_owner(std::string_view path, bool group)
{
// https://learn.microsoft.com/en-us/windows/win32/secauthz/finding-the-owner-of-a-file-object-in-c--

  PSECURITY_DESCRIPTOR pSD{nullptr};
  PSID pSid{nullptr};
  DWORD r;
  const std::wstring cpath{fs_win32_to_wide(path)};
  // https://learn.microsoft.com/en-us/windows/win32/api/aclapi/nf-aclapi-getnamedsecurityinfow
  if (group)
    r = GetNamedSecurityInfoW(cpath.c_str(), SE_FILE_OBJECT, GROUP_SECURITY_INFORMATION, nullptr, &pSid, nullptr, nullptr, &pSD);
  else
    r = GetNamedSecurityInfoW(cpath.c_str(), SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, &pSid, nullptr, nullptr, nullptr, &pSD);

  std::string s;
  if(r == ERROR_SUCCESS)
    s = fs_win32_get_owner(pSid);

  LocalFree(pSD);
  // https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-localfree
  return s;
}


#else

std::optional<uid_t> fs_stat_uid(std::string_view path)
{
#if defined(HAVE_STATX)
  return fs_internal::get_stat_value<std::optional<uid_t>>(
      path, {}, fs_internal::ignore_stat_error, STATX_UID,
      [](const struct statx& s) { return static_cast<uid_t>(s.stx_uid); },
      [](const struct stat& s) { return s.st_uid; });
#else
  return fs_internal::get_stat_value<std::optional<uid_t>>(
      path, {}, fs_internal::ignore_stat_error,
      [](const struct stat& s) { return s.st_uid; });
#endif
}


std::optional<gid_t> fs_stat_gid(std::string_view path)
{
#if defined(HAVE_STATX)
  return fs_internal::get_stat_value<std::optional<gid_t>>(
      path, {}, fs_internal::ignore_stat_error, STATX_GID,
      [](const struct statx& s) { return static_cast<gid_t>(s.stx_gid); },
      [](const struct stat& s) { return s.st_gid; });
#else
  return fs_internal::get_stat_value<std::optional<gid_t>>(
      path, {}, fs_internal::ignore_stat_error,
      [](const struct stat& s) { return s.st_gid; });
#endif
}

#endif

} // namespace


std::string
fs_get_owner_name(std::string_view path)
{
#if defined(_WIN32)
  if (std::string s = fs_win32_owner(path, false); !s.empty())
    return s;
#else
  if (auto uid = fs_stat_uid(path); uid) {
    if (auto pw = getpwuid(*uid))
      return pw->pw_name;
  }
#endif

  fs_error_callback(path);
  return {};
}


std::string
fs_get_owner_group(std::string_view path)
{
#if defined(_WIN32)
  if (std::string s = fs_win32_owner(path, true); !s.empty())
    return s;
#else
  if (auto gid = fs_stat_gid(path); gid) {
    if (auto gr = getgrgid(*gid))
      return gr->gr_name;
  }
#endif

  fs_error_callback(path);
  return {};
}
