#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include "ffilesystem.h"

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#endif

#include <string>
#include <string_view>

#include <system_error> // for std::error_code

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h> // IWYU pragma: keep
// GetTempPathA
#else
#include <unistd.h> // IWYU pragma: keep
// getcwd, chdir
#endif


std::string fs_get_tempdir()
{
  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM
  if(auto p = std::filesystem::temp_directory_path(ec); !ec) FFS_LIKELY
    return p.generic_string();
#else

  std::string path(fs_get_max_path(), '\0');
#ifdef _WIN32
  // GetTempPath2A is not in MSYS2
  auto L = GetTempPathA(static_cast<DWORD>(path.size()), path.data());
  if (L > 0 && L < path.size())  FFS_LIKELY
    path.resize(L);
#else
  path = fs_getenv("TMPDIR");
#endif

  if(!path.empty()) FFS_LIKELY
    return fs_as_posix(path);

  if (fs_is_dir("/tmp"))
    return "/tmp";

#endif

  fs_print_error("", "get_tempdir", ec);
  return {};
}


bool fs_set_cwd(std::string_view path)
{

  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM
  std::filesystem::current_path(path, ec);
  if(!ec) FFS_LIKELY
    return true;
#elif _WIN32
  // windows.h https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setcurrentdirectory
  if(SetCurrentDirectoryA(path.data()))  FFS_LIKELY
    return true;
#else
  // unistd.h https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/chdir-wchdir?view=msvc-170
  if(chdir(path.data()) == 0)  FFS_LIKELY
    return true;
#endif

  fs_print_error(path, "set_cwd", ec);
  return false;
}


std::string fs_get_cwd()
{

  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM
  if(auto s = std::filesystem::current_path(ec); !ec) FFS_LIKELY
    return s.generic_string();
#elif defined(_WIN32)
// windows.h https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getcurrentdirectory
  const DWORD L = GetCurrentDirectoryA(0, nullptr);
  if (L == 0){  FFS_UNLIKELY
    fs_print_error("", "get_cwd:GetCurrentDirectory");
    return {};
  }

  std::string r(L, '\0');
  if(GetCurrentDirectoryA(L, r.data()) == L-1){  FFS_LIKELY
    r.resize(L-1);
    return fs_as_posix(r);
  }
#else
// unistd.h https://www.man7.org/linux/man-pages/man3/getcwd.3.html
  if(std::string buf(fs_get_max_path(), '\0');
      getcwd(&buf[0], buf.size()))  FFS_LIKELY
    return fs_trim(buf);
#endif

  fs_print_error("", "get_cwd", ec);
  return {};
}
