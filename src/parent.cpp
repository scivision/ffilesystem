#include "ffilesystem.h"

#include <string>
#include <string_view>

#include <iostream>

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#elif defined(_WIN32)
#include <cstdlib> // _splitpath_s, _MAX_*
#else
#include <libgen.h> // dirname
#endif


std::string fs_parent(std::string_view path)
{

  std::string p(path);

#ifdef HAVE_CXX_FILESYSTEM
  p = fs_drop_slash(p);
  // have to drop trailing slash to get expected parent path -- necessary for AppleClang
  p = std::filesystem::path(p).parent_path().generic_string();

  if(fs_trace) std::cout << "TRACE:parent(" << path << ") => " << p << std::endl;

// 30.10.7.1 [fs.path.generic] dot-dot in the root-directory refers to the root-directory itself.
// On Windows, a drive specifier such as "C:" or "z:" is treated as a root-name.
// On Cygwin, a path that begins with two successive directory separators is a root-name.
// Otherwise (for POSIX-like systems other than Cygwin), the implementation-defined root-name
// is an unspecified string which does not appear in any pathnames.

#elif defined(_WIN32)
  p = fs_drop_slash(p);
  std::string dir(_MAX_DIR, '\0');
  std::string drive(_MAX_DRIVE, '\0');
  if(_splitpath_s(p.data(), drive.data(), _MAX_DRIVE, dir.data(), _MAX_DIR, nullptr, 0, nullptr, 0) != 0)
    return {};

  p = fs_drop_slash(fs_trim(drive) + fs_trim(dir));
#else
  char* d = dirname(p.data());
  return d ? d : "";
#endif

  if (p.empty())
    return ".";

  // need this for <filesystem> or _splitpath_s to make x: x:/
  if (fs_is_windows() && !p.empty() && p == fs_root_name(p))
    p.push_back('/');

  return p;
}
