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

#ifdef _WIN32
  std::string t(fs_get_max_path(), '\0');
  // GetTempPath2A is not in MSYS2
  auto L = GetTempPathA(static_cast<DWORD>(t.size()), t.data());
  if (L > 0)  FFS_LIKELY
    t.resize(L);
#else
  std::string t(fs_getenv("TMPDIR"));
#endif

  if(!t.empty()) FFS_LIKELY
    return fs_as_posix(t);

  if (fs_is_dir("/tmp"))
    return "/tmp";

#endif

  fs_print_error("", __func__, ec);
  return {};
}
