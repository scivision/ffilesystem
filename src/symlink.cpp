#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "ffilesystem.h"

#include <iostream>

#include <string>
#include <string_view>

#include <system_error>


#if defined(HAVE_BOOST_FILESYSTEM)
#include <boost/filesystem.hpp>
#elif defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
#else
// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>

#ifndef _WIN32
#include <unistd.h> // readlink(), symlink()
#endif

#if defined(__linux__) && defined(USE_STATX)
#include <fcntl.h>   // AT_* constants for statx()
#endif

#endif


bool fs_is_symlink(std::string_view path)
{

  Fserr::error_code ec;

#if defined(__MINGW32__) || (defined(_WIN32) && !defined(HAVE_CXX_FILESYSTEM) && !defined(HAVE_BOOST_FILESYSTEM))
  if (const DWORD a = GetFileAttributesA(path.data());
        a != INVALID_FILE_ATTRIBUTES)  FFS_LIKELY
    return a & FILE_ATTRIBUTE_REPARSE_POINT;
#elif defined(HAVE_CXX_FILESYSTEM) || defined(HAVE_BOOST_FILESYSTEM)
// Fs::symlink_status doesn't detect symlinks on MinGW
  const auto s = Fs::symlink_status(path, ec);
  if(!ec) FFS_LIKELY
    return Fs::is_symlink(s);

#elif defined(STATX_MODE) && defined(USE_STATX)
// Linux Glibc only
// https://www.gnu.org/software/gnulib/manual/html_node/statx.html
// https://www.man7.org/linux/man-pages/man2/statx.2.html
  if (FS_TRACE) std::cout << "TRACE: statx() is_symlink " << path << "\n";
  struct statx s;
  if(statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW, STATX_MODE, &s) == 0) FFS_LIKELY
    return S_ISLNK(s.stx_mode);
#else
  struct stat s;

  if(!lstat(path.data(), &s)) FFS_LIKELY
    return S_ISLNK(s.st_mode);
  // return (s.st_mode & S_IFMT) == S_IFLNK; // equivalent
#endif

  fs_print_error(path, "is_symlink", ec);
  return false;
}


std::string fs_read_symlink(std::string_view path)
{

  if(!fs_is_symlink(path)){ FFS_UNLIKELY
    std::cerr << "ERROR:Ffilesystem:read_symlink(" << path << ") is not a symlink\n";
    return {};
  }

  Fserr::error_code ec;

#if defined(__MINGW32__) || (defined(_WIN32) && !defined(HAVE_CXX_FILESYSTEM) && !defined(HAVE_BOOST_FILESYSTEM))
  return fs_win32_final_path(path);
#elif defined(HAVE_CXX_FILESYSTEM) || defined(HAVE_BOOST_FILESYSTEM)
  if(auto p = Fs::read_symlink(path, ec); !ec) FFS_LIKELY
    return p.generic_string();
#else
  // https://www.man7.org/linux/man-pages/man2/readlink.2.html
  std::string r(fs_get_max_path(), '\0');

  const ssize_t Lr = readlink(path.data(), r.data(), r.size());
  if (Lr > 0){
    // readlink() does not null-terminate the result
    r.resize(Lr);
    return r;
  }
#endif

  fs_print_error(path, "read_symlink", ec);
  return {};
}


bool fs_create_symlink(std::string_view target, std::string_view link)
{

  if(target.empty()) FFS_UNLIKELY
  {
    std::cerr << "ERROR: create_symlink: target path must not be empty\n";
    // confusing program errors if target is "" -- we'd never make such a symlink in real use.
    return false;
  }

  if(link.empty()) FFS_UNLIKELY
  {
    std::cerr << "ERROR: create_symlink: link path must not be empty\n";
    // macOS needs empty check to avoid SIGABRT
    return false;
  }

  Fserr::error_code ec;

#if defined(__MINGW32__) || (defined(_WIN32) && !defined(HAVE_CXX_FILESYSTEM) && !defined(HAVE_BOOST_FILESYSTEM))

  DWORD p = SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;

  if(fs_is_dir(target.data()))
    p |= SYMBOLIC_LINK_FLAG_DIRECTORY;

  if(CreateSymbolicLinkA(link.data(), target.data(), p))  FFS_LIKELY
    return true;

#elif defined(HAVE_CXX_FILESYSTEM) || defined(HAVE_BOOST_FILESYSTEM)

  fs_is_dir(target)
    ? Fs::create_directory_symlink(target, link, ec)
    : Fs::create_symlink(target, link, ec);

  if(!ec) FFS_LIKELY
    return true;

#else
  // https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/symlink.2.html
  // https://linux.die.net/man/3/symlink
  if(symlink(target.data(), link.data()) == 0)  FFS_LIKELY
    return true;

#endif

  fs_print_error(target, link, "create_symlink", ec);
  return false;
}
