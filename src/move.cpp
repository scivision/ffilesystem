#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
#else

#include <cstdio> // for std::remove, std::rename

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#endif

#include <string_view>
#include <system_error>         // for error_code

#include "ffilesystem.h"


bool
fs_remove(std::string_view path)
{
  // remove a file or empty directory
  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM
  // https://en.cppreference.com/w/cpp/filesystem/remove
  if(std::filesystem::remove(path, ec) && !ec) FFS_LIKELY
    return true;
#else
  // https://en.cppreference.com/w/cpp/io/c/remove
  if(std::remove(path.data()) == 0)
    return true;

#ifdef _WIN32
  // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-deletefilea
  // may need these when deleting a symlink
  // https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createsymboliclinka#remarks
  std::wstring const w = fs_win32_to_wide(path);

  if(fs_is_file(path)){
    if(DeleteFileW(w.data()) != 0)
      return true;
  } else if(fs_is_dir(path)){
    if(RemoveDirectoryW(w.data()) != 0)
      return true;
  }

#endif

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

#ifdef HAVE_CXX_FILESYSTEM
  // https://en.cppreference.com/w/cpp/filesystem/rename
  std::filesystem::rename(from, to, ec);
  if(!ec)
#else
  // https://en.cppreference.com/w/cpp/io/c/rename
  if(std::rename(from.data(), to.data()) == 0)
#endif
    return true;

  fs_print_error(from, to, __func__, ec);
  return false;

}
