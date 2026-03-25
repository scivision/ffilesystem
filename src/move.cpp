#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Filesystem = std::filesystem;
#else

#include <cstdio> // for _unlink, std::rename

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h> // for rmdir
#define unlink _unlink
#define rmdir _rmdir
#else
#include <unistd.h> // for unlink
#endif

#endif

#include <string_view>
#include <system_error>         // for error_code

#include "ffilesystem.h"

#include <iostream>

bool
fs_remove(std::string_view path)
{
  // remove a file or empty directory
  std::error_code ec;

#if defined(HAVE_CXX_FILESYSTEM)
  // https://en.cppreference.com/w/cpp/filesystem/remove
  if(Filesystem::remove(path, ec) && !ec) FFS_LIKELY
    return true;
#else
   std::string cpath(path);
  // https://en.cppreference.com/w/cpp/io/c/remove
  if (fs_is_dir(path)) {
    // directory must be empty
    // https://www.man7.org/linux/man-pages/man2/rmdir.2.html
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/rmdir-wrmdir
    if (rmdir(cpath.c_str()) == 0)
      return true;
  } else if(unlink(cpath.c_str()) == 0)
      return true;
#endif

  fs_print_error(path, __func__, ec);
  return false;
}


bool
fs_rename(std::string_view from, std::string_view to)
{
  // rename a file or directory
  // existing "to" is overwritten

  std::error_code ec;

#if defined(HAVE_CXX_FILESYSTEM)
  // https://en.cppreference.com/w/cpp/filesystem/rename
  if(Filesystem::rename(from, to, ec); !ec)
#else
  // https://en.cppreference.com/w/cpp/io/c/rename
  if(std::rename(from.data(), to.data()) == 0)
#endif
    return true;

  fs_print_error(from, to, __func__, ec);
  return false;

}
