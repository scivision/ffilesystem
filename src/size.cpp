#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include "ffilesystem.h"

#include <string_view>
#include <system_error>

#include <cstdint>  // uintmax_t

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#else
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dirent.h>  // opendir, readdir, closedir
#endif

#if defined(__linux__) && defined(USE_STATX)
#include <fcntl.h>   // AT_* constants for statx()
#endif

#endif


std::uintmax_t fs_file_size(std::string_view path)
{
  std::error_code ec;
#ifdef HAVE_CXX_FILESYSTEM
  if(auto s = std::filesystem::file_size(path, ec); !ec)  FFS_LIKELY
    return s;
#elif defined(_WIN32)
  // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfilesizeex
  if (HANDLE h = CreateFileA(path.data(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
      h != INVALID_HANDLE_VALUE)
  {
    LARGE_INTEGER size;
    BOOL ok = GetFileSizeEx(h, &size);

    if (CloseHandle(h) && ok)  FFS_LIKELY
      return size.QuadPart;
  } else {
    ec = std::make_error_code(std::errc::no_such_file_or_directory);
  }
#else

  int r = 0;
#if defined(STATX_SIZE) && defined(USE_STATX)
  struct statx sx;
  r = statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_SIZE, &sx);
  if (r == 0) FFS_LIKELY
    return sx.stx_size;
#endif

  if (r == 0 || errno == ENOSYS){
    if (struct stat s; !stat(path.data(), &s))
      return s.st_size;
  }

#endif

  fs_print_error(path, __func__, ec);
  return {};
}


bool fs_is_empty(std::string_view path)
{
  // directory or file empty
  // returns false if path doesn't exist

  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM
  if (bool e = std::filesystem::is_empty(path, ec); !ec)  FFS_LIKELY
    return e;
#else

  if (!fs_is_dir(path))
    return fs_file_size(path) == 0 && fs_is_file(path);

  // directory empty
#if defined(_WIN32)
  // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstfilea
  WIN32_FIND_DATAA ffd;
  HANDLE hFind = FindFirstFileA((std::string(path) + "/*").data(), &ffd);
  if (hFind == INVALID_HANDLE_VALUE) {
    fs_print_error(path, __func__);
    return false;
  }

  do
  {
      if(fs_trace) std::cout << "TRACE: is_empty: do " << ffd.cFileName << "\n";

      if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        // std::set is much slower than a simple if
        if (std::string_view n(ffd.cFileName); n == "." || n == "..")
          continue;

      // directory that is not . or ..
        FindClose(hFind);
        return false;
      }
      // any non-directory
      FindClose(hFind);
      return false;
  } while (FindNextFileA(hFind, &ffd));

  // empty directory
  FindClose(hFind);
  return true;
#else
// https://www.man7.org/linux/man-pages/man3/opendir.3.html
// https://www.man7.org/linux/man-pages/man3/readdir.3.html
// https://www.man7.org/linux/man-pages/man3/closedir.3.html
// https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/readdir.3.html

  if (DIR *d = opendir(path.data()); d)
  {
    struct dirent *entry;
  while ((entry = readdir(d)))
  {
#ifdef _DIRENT_HAVE_D_TYPE
    if (entry->d_type == DT_DIR)
#else
    if (fs_is_dir(std::string(path) + "/" + entry->d_name))
#endif
    {
      // std::set is much slower than a simple if
      if (std::string_view n(entry->d_name); n == "." || n == "..")
        continue;
      // directory that is not . or ..
      closedir(d);
      return false;
    }
    // any non-directory
    closedir(d);
    return false;
  }
    // empty directory
    closedir(d);
    return true;
  }

#endif

#endif

  fs_print_error(path, __func__, ec);
  return false;

}
