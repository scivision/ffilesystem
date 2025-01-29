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


#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#else
#include <sys/types.h> // ssize_t
#include <sys/stat.h> // stat(), statx()

#if !defined(_WIN32)
#include <unistd.h> // readlink(), symlink()
#endif

#if defined(__linux__) && defined(USE_STATX)
#include <fcntl.h>   // AT_* constants for statx()
#endif

#endif


bool fs_is_symlink(std::string_view path)
{

#if defined(__MINGW32__) || (defined(_WIN32) && !defined(HAVE_CXX_FILESYSTEM))
  return fs_win32_is_symlink(path);
#elif defined(HAVE_CXX_FILESYSTEM)
// std::filesystem::symlink_status or std::filesystem::is_symlink
// don't detect symlinks on MinGW
  std::error_code ec;
  return std::filesystem::is_symlink(path, ec) && !ec;
#elif defined(STATX_MODE) && defined(USE_STATX)
// Linux Glibc only
// https://www.gnu.org/software/gnulib/manual/html_node/statx.html
// https://www.man7.org/linux/man-pages/man2/statx.2.html
  if (fs_trace) std::cout << "TRACE: statx() is_symlink " << path << "\n";
  struct statx s;
  return statx(AT_FDCWD, path.data(),
               AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW,
               STATX_MODE, &s) == 0 &&
         S_ISLNK(s.stx_mode);
#else
  struct stat s;
  return lstat(path.data(), &s) == 0 && S_ISLNK(s.st_mode);
#endif
}


std::string fs_read_symlink(std::string_view path)
{
  // read the target of a symlink

  std::error_code ec = std::make_error_code(std::errc::invalid_argument);

#if defined(__MINGW32__) || (defined(_WIN32) && !defined(HAVE_CXX_FILESYSTEM))
  if(fs_is_symlink(path))
    return fs_win32_final_path(path);
#elif defined(HAVE_CXX_FILESYSTEM)
  if(auto p = std::filesystem::read_symlink(path, ec); !ec) FFS_LIKELY
    return p.generic_string();
#else
  // https://www.man7.org/linux/man-pages/man2/readlink.2.html
  std::string r(fs_get_max_path(), '\0');

  if (ssize_t Lr = readlink(path.data(), r.data(), r.size()); Lr > 0){
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
  // create symlink to file or directory

  std::error_code ec;

  // confusing program errors if target is "" -- we'd never make such a symlink in real use.
  // macOS needs empty check to avoid SIGABRT

  if(target.empty() || link.empty()) FFS_UNLIKELY
    ec = std::make_error_code(std::errc::invalid_argument);
  else {
#if defined(__MINGW32__) || (defined(_WIN32) && !defined(HAVE_CXX_FILESYSTEM))

  DWORD p = SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;

  if(fs_is_dir(target.data()))
    p |= SYMBOLIC_LINK_FLAG_DIRECTORY;

  if(CreateSymbolicLinkA(link.data(), target.data(), p))  FFS_LIKELY
    return true;

#elif defined(HAVE_CXX_FILESYSTEM)

  fs_is_dir(target)
    ? std::filesystem::create_directory_symlink(target, link, ec)
    : std::filesystem::create_symlink(target, link, ec);

  if(!ec) FFS_LIKELY
    return true;

#else
  // https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/symlink.2.html
  // https://linux.die.net/man/3/symlink
  if(symlink(target.data(), link.data()) == 0)  FFS_LIKELY
    return true;
#endif
  }

  fs_print_error(target, link, "create_symlink", ec);
  return false;
}
