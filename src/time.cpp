// IWYU pragma: no_include <bits/statx-generic.h>
// IWYU pragma: no_include <linux/stat.h>
// IWYU pragma: no_include <bits/chrono.h>

#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include "ffilesystem.h"

#include <iostream>
#include <optional>
#include <system_error>

#ifdef HAVE_CLOCK_CAST
// this include can break even when functions not used if C++ ABI problem e.g. Clang 14 with GCC 13
#include <chrono>
#endif
#include <ctime>                // for time_t

// always include to avoid dev confusion
#include <sys/types.h> // IWYU pragma: keep
#include <sys/stat.h> // statx(), stat()

#include <string_view>

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

// standalone #if
#if !defined(_WIN32)
#include <fcntl.h> // utimensat, AT_* constants
#endif


std::time_t fs_get_modtime(std::string_view path)
{

#if defined(HAVE_CLOCK_CAST) && defined(HAVE_CXX_FILESYSTEM)
  if(const auto &t_fs = fs_get_modtime_fs(path); t_fs){
    const auto t_sys = std::chrono::clock_cast<std::chrono::system_clock>(t_fs.value());
    return std::chrono::system_clock::to_time_t(t_sys);
  }
#elif defined(STATX_MTIME) && defined(USE_STATX)
// https://www.man7.org/linux/man-pages/man2/statx.2.html
  struct statx s;

  if( statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_MTIME, &s) == 0 ) FFS_LIKELY
    return s.stx_mtime.tv_sec;
#else
  if (struct stat s; !stat(path.data(), &s))
    return s.st_mtime;
#endif

  fs_print_error(path, __func__);
  return {};
}


#ifdef HAVE_CXX_FILESYSTEM
std::optional<std::filesystem::file_time_type> fs_get_modtime_fs(std::string_view path)
{
  std::error_code ec;

  if(std::filesystem::file_time_type t_fs = std::filesystem::last_write_time(path, ec); !ec) FFS_LIKELY
    return t_fs;

  fs_print_error(path, __func__, ec);
  return {};
}
#endif


bool fs_set_modtime(std::string_view path, const bool quiet)
{

  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM

  std::filesystem::last_write_time(path, std::filesystem::file_time_type::clock::now(), ec);
  if(!ec) FFS_LIKELY
    return true;
  // techinically IWYU <chrono> but that can break some compilers, and it works without the include.

#elif defined(_WIN32)
// https://learn.microsoft.com/en-us/windows/win32/SysInfo/changing-a-file-time-to-the-current-time
  HANDLE h = CreateFileA(path.data(),
                         FILE_WRITE_ATTRIBUTES, FILE_SHARE_WRITE, nullptr,
                         OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
  if (h){
    FILETIME t;
    // https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemtimeasfiletime
    GetSystemTimeAsFileTime(&t);
    BOOL ok = SetFileTime(h, nullptr, nullptr, &t);

    if(!CloseHandle(h))
      ec = std::make_error_code(std::errc::io_error);
    else if (ok) FFS_LIKELY
      return true;
  }
#else
  // utimensat available in macOS since 10.13
  // https://github.com/python/cpython/issues/75782
  // https://gitlab.kitware.com/cmake/cmake/-/issues/17101
  if (utimensat(AT_FDCWD, path.data(), nullptr, 0) == 0)  FFS_LIKELY
    return true;
#endif

  if (!quiet)
    fs_print_error(path, __func__, ec);

  return false;
}
