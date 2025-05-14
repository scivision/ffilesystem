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


#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Filesystem = std::filesystem;
#else

#if !defined(_WIN32)
#include <unistd.h> // readlink(), symlink()
#endif

#endif


#if defined(__linux__) && defined(USE_STATX)
#include <fcntl.h>   // AT_* constants for statx()
#endif

#include <sys/types.h> // ssize_t
#include <sys/stat.h> // stat(), statx()


std::string::size_type fs_symlink_length([[maybe_unused]] std::string_view path)
{
  // get the string length of a symlink target
  // falls back to maximum path length

  std::string::size_type L = 0;

#if !defined(_WIN32)
  int r = 0;

#if defined(STATX_SIZE) && defined(USE_STATX)
  struct statx sx;
  r = statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW, STATX_SIZE, &sx);
  if (r == 0)
    L = sx.stx_size;
#endif
// https://linux.die.net/man/2/lstat

  if(r == 0 || errno == ENOSYS){
    if(struct stat s; lstat(path.data(), &s) == 0)
      L = s.st_size;
  }
#endif

  return (L > 0) ? L + 1 : fs_get_max_path();
}


bool fs_is_symlink(std::string_view path)
{
  std::error_code ec;

#if defined(__MINGW32__) || (defined(_WIN32) && !defined(HAVE_CXX_FILESYSTEM))
  return fs_win32_is_symlink(path);
#elif defined(HAVE_CXX_FILESYSTEM)
// std::filesystem::symlink_status or std::filesystem::is_symlink
// don't detect symlinks on MinGW

  if(bool is_sym = Filesystem::is_symlink(path, ec); !ec)
    return is_sym;
#else

  int r = 0;

#if defined(STATX_MODE) && defined(USE_STATX)
// Linux Glibc only
// https://www.gnu.org/software/gnulib/manual/html_node/statx.html
// https://www.man7.org/linux/man-pages/man2/statx.2.html

  struct statx sx;
  r = statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW, STATX_MODE, &sx);
  if (r == 0) FFS_LIKELY
    return S_ISLNK(sx.stx_mode);
#endif
// https://linux.die.net/man/2/lstat

  if(r == 0 || errno == ENOSYS){
    if(struct stat s; lstat(path.data(), &s) == 0)
      return S_ISLNK(s.st_mode);
  }

#endif

  fs_print_error(path, __func__, ec);
  return false;
}


std::string fs_read_symlink(std::string_view path)
{
  // read the target of a symlink

  std::error_code ec = std::make_error_code(std::errc::invalid_argument);

#if defined(__MINGW32__) || (defined(_WIN32) && !defined(HAVE_CXX_FILESYSTEM))
  if(fs_is_symlink(path))
    return fs_win32_final_path(path);
#elif defined(HAVE_CXX_FILESYSTEM)
  if(auto p = Filesystem::read_symlink(path, ec); !ec) FFS_LIKELY
    return p.generic_string();
#else

  // https://www.man7.org/linux/man-pages/man2/readlink.2.html
  // https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/readlink.2.html

  std::string::size_type L = fs_symlink_length(path);
  std::string p;
  p.resize(L);

  if (ssize_t Lr = readlink(path.data(), p.data(), p.size());
      Lr == static_cast<ssize_t>(L-1)){
    // readlink() does not null-terminate the result
    p.resize(Lr);
    return p;
  }
#endif

  fs_print_error(path, __func__, ec);
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

  if(CreateSymbolicLinkW(fs_win32_to_wide(link).data(), fs_win32_to_wide(target).data(), p))
    return true;

#elif defined(HAVE_CXX_FILESYSTEM)

  if(fs_is_dir(target) ? Filesystem::create_directory_symlink(target, link, ec) : Filesystem::create_symlink(target, link, ec); !ec)
    return true;

#else
  // https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/symlink.2.html
  // https://linux.die.net/man/3/symlink
  if(symlink(target.data(), link.data()) == 0)
    return true;
#endif
  }

  fs_print_error(target, link, __func__, ec);
  return false;
}
