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
#endif

#include "ffilesystem.h"


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

  fs_print_error("", __func__, ec);
  return {};
}
