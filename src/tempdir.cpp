#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Filesystem = std::filesystem;
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
  if(auto p = Filesystem::temp_directory_path(ec); !ec && !p.empty())
    return p.string();
#endif

#if defined(_WIN32)
  // GetTempPath2 is not in MSYS2. libuv etc. use GetTempPathW
  if(DWORD L = GetTempPathW(0, nullptr); L > 0) {
    std::wstring w;
    w.resize(L + 1);
    if(GetTempPathW(L, w.data()) == L) {
      w.resize(L);
      return fs_win32_to_narrow(w);
    }
    if (L > 0) {
      w.resize(L);
      return fs_win32_to_narrow(w);
    }
  }
#else
  if(auto t = fs_getenv("TMPDIR"); t.has_value() && !t.value().empty())
    return t.value();

  if (fs_is_dir("/tmp"))
    return "/tmp";
#endif

  fs_print_error("", __func__, ec);
  return {};

}
