#include <string>
#include <string_view>

#include "ffilesystem.h"

#ifdef __CYGWIN__
#include <sys/cygwin.h>
#else
constexpr int CCP_WIN_A_TO_POSIX = 0;
constexpr int CCP_POSIX_TO_WIN_A = 0;
#endif


static std::string fs_convert_path(std::string_view path, FFS_MAYBE_UNUSED int const what)
{
#ifdef __CYGWIN__
  const auto L = cygwin_conv_path(what, path.data(), nullptr, 0);
  if(L > 0){
    if (std::string r(L, '\0'); !cygwin_conv_path(what, path.data(), r.data(), L))
      return r;
  }
#endif

  fs_print_error(path, __func__);
  return {};
}

std::string fs_to_cygpath(std::string_view path) {
  return fs_convert_path(path, CCP_WIN_A_TO_POSIX);
}

std::string fs_to_winpath(std::string_view path) {
  return fs_convert_path(path, CCP_POSIX_TO_WIN_A);
}
