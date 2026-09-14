#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Filesystem = std::filesystem;
#endif

#include "ffilesystem.h"

#include <string>
#include <string_view>

#include <system_error> // for std::error_code

#include <iostream> // IWYU pragma: keep

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h> // IWYU pragma: keep
// GetTempPathA
#elif defined(FFS_DARWIN)
#include <unistd.h> // for confstr
#endif



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
    std::wstring w(L, '\0');
    // null-term'd https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-gettemppathw
    if(GetTempPathW(L, w.data()) == L-1)
      return fs_win32_to_narrow(w);
  }
#elif defined(FFS_DARWIN)
// https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/confstr.3.html
  std::size_t L = ::confstr(_CS_DARWIN_USER_TEMP_DIR, nullptr, 0);
  if (L > 1) {
    std::string t(L, '\0');
    if(::confstr(_CS_DARWIN_USER_TEMP_DIR, t.data(), L) == L) {
      return t.substr(0, L-1);
    }
  }
#endif

#if !defined(_WIN32)
  auto t = fs_getenv("TMPDIR");
  std::string tempdir{t.value_or("/tmp")};

  if (fs_is_dir(tempdir))
    return tempdir;
#endif

  fs_error_callback("", ec);
  return {};

}
