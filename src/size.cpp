#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include "ffilesystem.h"

#include <string_view>
#include <system_error>

#include <cstdint>  // uintmax_t

#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Filesystem = std::filesystem;
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

#if __has_include(<fcntl.h>)
#include <fcntl.h>   // AT_* constants for statx
#endif

#endif


std::uintmax_t fs_file_size(std::string_view path)
{
  std::error_code ec;
#if defined(HAVE_CXX_FILESYSTEM)

  auto s = Filesystem::file_size(path, ec);
  if (ec)
    fs_print_error(path, __func__, ec);

  return s;

#elif defined(_WIN32)
  // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfilesizeex

  HANDLE h = CreateFileW(fs_win32_to_wide(path).data(),
                             GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

  if (h != INVALID_HANDLE_VALUE) {
    LARGE_INTEGER size;
    BOOL ok = GetFileSizeEx(h, &size);
    CloseHandle(h);
    if (ok)
      return size.QuadPart;
  }

  ec = std::make_error_code(std::errc::no_such_file_or_directory);
#else

  int r = 0;
#if defined(HAVE_STATX)
  struct statx sx;
  r = ::statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_SIZE, &sx);
  if (r == 0)
    return sx.stx_size;
#endif

  if (r == 0 || errno == ENOSYS){
    if (struct stat s; !::stat(path.data(), &s))
      return s.st_size;
  }

#endif

  fs_print_error(path, __func__, ec);
  return static_cast<std::uintmax_t>(-1);
}


bool fs_is_empty(std::string_view path)
{
  // directory or file empty
  // returns false if path doesn't exist

  std::error_code ec;

#if defined(HAVE_CXX_FILESYSTEM)
  if (bool e = Filesystem::is_empty(path, ec); !ec)
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

  // RAII for FindClose
  struct FindHandleCloser {
    HANDLE h;
    ~FindHandleCloser() { if (h != INVALID_HANDLE_VALUE) FindClose(h); }
  } _closer{hFind};

  do
  {
      if(fs_trace) std::cout << "TRACE: is_empty: do " << ffd.cFileName << "\n";

      if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        // std::set is much slower than a simple if
        if (std::string_view n(ffd.cFileName); n == "." || n == "..")
          continue;

      // directory that is not . or ..
        return false;
      }
      // any non-directory
      return false;
  } while (FindNextFileA(hFind, &ffd));

  // empty directory
  return true;
#else
// https://www.man7.org/linux/man-pages/man3/opendir.3.html
// https://www.man7.org/linux/man-pages/man3/readdir.3.html
// https://www.man7.org/linux/man-pages/man3/closedir.3.html
// https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/readdir.3.html

  if (DIR *d = ::opendir(path.data()); d)
  {
    // RAII for closedir
    struct DirCloser { DIR* d; ~DirCloser(){ if(d) ::closedir(d); } } _dc{d};
    struct dirent *entry;
  while ((entry = ::readdir(d)))
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
      return false;
    }
    // any non-directory
    return false;
  }
    // empty directory
    return true;
  }

#endif

#endif

  fs_print_error(path, __func__, ec);
  return false;

}
