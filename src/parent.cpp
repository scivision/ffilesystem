#include "ffilesystem.h"

#include <string>
#include <string_view>

#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Filesystem = std::filesystem;
#elif defined(_WIN32)
#include "win32_path.h"
#else
#include <libgen.h> // for dirname
#endif


std::string fs_parent(std::string_view path)
{

  std::string p(path);

#if defined(HAVE_CXX_FILESYSTEM)
  fs_drop_trailing_slash(p);
  // have to drop trailing slash to get expected parent path -- necessary for AppleClang
  p = Filesystem::path(p).parent_path().string();

// 30.10.7.1 [fs.path.generic] dot-dot in the root-directory refers to the root-directory itself.
// On Windows, a drive specifier such as "C:" or "z:" is treated as a root-name.
// On Cygwin, a path that begins with two successive directory separators is a root-name.
// Otherwise (for POSIX-like systems other than Cygwin), the implementation-defined root-name
// is an unspecified string which does not appear in any pathnames.

#elif defined(_WIN32)
  fs_drop_trailing_slash(p);
  const auto parts = fs_win32_split_path(p);
  if (!parts)
    return {};

  p = parts->drive + parts->directory;
  fs_drop_trailing_slash(p);
#else
  // https://linux.die.net/man/3/dirname
  // https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/dirname.3.html
  // result is checked to see if nullptr. This method is clear and simple.
  char* d = dirname(p.data());
  return d ? d : "";
#endif

  if (p.empty())
    return ".";

  // need this for <filesystem> or _splitpath_s to make x: x:/
  if (fs_is_windows() && p == fs_root_name(p))
    p.push_back('/');

  return p;
}
