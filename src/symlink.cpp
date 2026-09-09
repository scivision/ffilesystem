#if defined(__linux__) || defined(__CYGWIN__)
#if !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE // for readlink
#endif
#endif

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "ffilesystem.h"

#include <iostream>

#include <string>
#include <string_view>

#include <system_error>
#include <cerrno>


#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Filesystem = std::filesystem;
#else

#if !defined(_WIN32)
#include <unistd.h> // readlink(), symlink()
#endif

#endif

// MinGW GCC <filesystem> symlink functions don't work until GCC 17
// https://gcc.gnu.org/gcc-17/changes.html#libstdcxx
#if (defined(__MINGW32__) && !defined(__clang__) && (defined(__GNUC__) && (__GNUC__ < 17))) || (defined(_WIN32) && !defined(HAVE_CXX_FILESYSTEM))
#define FS_USE_WIN32_SYMLINK 1
#else
#define FS_USE_WIN32_SYMLINK 0
#endif


#if __has_include(<fcntl.h>)
#include <fcntl.h>   // AT_* constants for statx
#endif

#include <sys/types.h> // ssize_t
#include <sys/stat.h> // for stat, statx


std::string_view::size_type fs_symlink_length([[maybe_unused]] std::string_view path)
{
  // get the string length of a symlink target
  // falls back to maximum path length

  std::string_view::size_type L = 0;

#if !defined(_WIN32)
  const std::string cpath{path};

#if defined(HAVE_STATX)
  if(struct statx x; ::statx(AT_FDCWD, cpath.c_str(), AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW, STATX_SIZE, &x) == 0)
    L = x.stx_size;
  else if (errno != ENOSYS)
    return fs_get_max_path();
#endif
// https://linux.die.net/man/2/lstat

  if(struct stat s; ::lstat(cpath.c_str(), &s) == 0)
    L = s.st_size;
#endif

  return (L > 0) ? L + 1 : fs_get_max_path();
}


bool fs_is_symlink(std::string_view path)
{
  std::error_code ec;

  auto handle_error = [&]() {
    fs_print_error(path, ec);
    return false;
  };

#if FS_USE_WIN32_SYMLINK
  return fs_win32_is_symlink(path);
#elif defined(HAVE_CXX_FILESYSTEM)
  if(bool is_sym = Filesystem::is_symlink(path, ec); !ec)
    return is_sym;
#else

  const std::string cpath{path};

#if defined(HAVE_STATX)
// Linux Glibc only
// https://www.gnu.org/software/gnulib/manual/html_node/statx.html
// https://www.man7.org/linux/man-pages/man2/statx.2.html

  if(struct statx x; ::statx(AT_FDCWD, cpath.c_str(), AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW, STATX_MODE, &x) == 0)
    return S_ISLNK(x.stx_mode);
  else if (errno != ENOSYS)
    return handle_error();
#endif
// https://linux.die.net/man/2/lstat

  if(struct stat s; ::lstat(cpath.c_str(), &s) == 0)
    return S_ISLNK(s.st_mode);

#endif

  return handle_error();
}


bool fs_lexists(std::string_view path)
{
  // fs_lexists() is true for broken symlinks, unlike fs_exists()
#if defined(HAVE_CXX_FILESYSTEM) && !FS_USE_WIN32_SYMLINK
  std::error_code ec;
  const auto s = Filesystem::symlink_status(path, ec);
  return !ec && (Filesystem::exists(s) || Filesystem::is_symlink(s));
#else
  // For broken symlinks, fs_exists() returns false but fs_is_symlink() returns true
  return fs_exists(path) || fs_is_symlink(path);
#endif
}


std::string fs_read_symlink(std::string_view path)
{
  // read the target of a symlink

  std::error_code ec = std::make_error_code(std::errc::invalid_argument);

#if FS_USE_WIN32_SYMLINK
  if(fs_is_symlink(path))
    return fs_win32_final_path(path);
#elif defined(HAVE_CXX_FILESYSTEM)
  if(auto p = Filesystem::read_symlink(path, ec); !ec)
    return p.string();
#else

  // https://www.man7.org/linux/man-pages/man2/readlink.2.html
  // https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/readlink.2.html

  std::string::size_type L = fs_symlink_length(path);
  std::string p(L, '\0');

  const std::string cpath{path};

  if (ssize_t Lr = ::readlink(cpath.c_str(), p.data(), p.size());
      Lr == static_cast<ssize_t>(L-1)){
    // readlink() does not null-terminate the result
    p.resize(Lr);
    return p;
  }
#endif

  fs_print_error(path, ec);
  return {};
}


bool fs_create_symlink(std::string_view target, std::string_view link)
{
  // create symlink to file or directory

  std::error_code ec;

  // confusing program errors if target is "" -- we'd never make such a symlink in real use.
  // macOS needs empty check to avoid SIGABRT

  if(target.empty() || link.empty())
    ec = std::make_error_code(std::errc::invalid_argument);
  else {
#if FS_USE_WIN32_SYMLINK
  DWORD p = SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;

  if(fs_is_dir(target))
    p |= SYMBOLIC_LINK_FLAG_DIRECTORY;

  if(CreateSymbolicLinkW(fs_win32_to_wide(link).c_str(), fs_win32_to_wide(target).c_str(), p))
    return true;

#elif defined(HAVE_CXX_FILESYSTEM)

  if(fs_is_dir(target) ? Filesystem::create_directory_symlink(target, link, ec) : Filesystem::create_symlink(target, link, ec); !ec)
    return true;

#else
  // https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/symlink.2.html
  // https://linux.die.net/man/3/symlink
  const std::string tgt{target};
  const std::string clink{link};
  if(::symlink(tgt.c_str(), clink.c_str()) == 0)
    return true;
#endif
  }

  fs_print_error(target, link, ec);
  return false;
}
