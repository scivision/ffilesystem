#include <string>
#include <string_view>

#include "ffilesystem.h"
#include "internal.h"

#include <system_error>

#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Filesystem = std::filesystem;
#else
#include <sys/types.h>
#include <sys/stat.h> // chmod, permissions constants
# if defined(_WIN32)
# define WIN32_LEAN_AND_MEAN
# include <io.h> // for chmod
#  ifndef S_IRUSR
   constexpr mode_t S_IRUSR = _S_IREAD;
#  endif
#  ifndef S_IWUSR
   constexpr mode_t S_IWUSR = _S_IWRITE;
#  endif
#  ifndef S_IXUSR
   constexpr mode_t S_IXUSR = _S_IEXEC;
#  endif
# endif
#endif


namespace ffilesystem {

bool set_permissions(std::string_view path, const int readable, const int writable, const int executable)
{
// only sets permission for owner, not group or others

#ifdef HAVE_CXX_FILESYSTEM

#if defined(__cpp_using_enum)  // C++20
  using enum Filesystem::perms;
#else
  constexpr Filesystem::perms owner_read = Filesystem::perms::owner_read;
  constexpr Filesystem::perms owner_write = Filesystem::perms::owner_write;
  constexpr Filesystem::perms owner_exec = Filesystem::perms::owner_exec;
#endif

  std::error_code ec;
  // need to error if path doesn't exist and no operations are requested
  if(!exists(path))
    ec = std::make_error_code(std::errc::no_such_file_or_directory);

  if (!ec && readable != 0)
    Filesystem::permissions(path, owner_read,
      (readable > 0) ? Filesystem::perm_options::add : Filesystem::perm_options::remove,
      ec);

  if (!ec && writable != 0)
    Filesystem::permissions(path, owner_write,
      (writable > 0) ? Filesystem::perm_options::add : Filesystem::perm_options::remove,
      ec);

  if (!ec && executable != 0)
    Filesystem::permissions(path, owner_exec,
      (executable > 0) ? Filesystem::perm_options::add : Filesystem::perm_options::remove,
      ec);

  if(!ec) FFS_LIKELY
    return true;

  fs_error_callback(path, ec);
  return false;

#else

  mode_t m = fs_st_mode(path);

  if(readable > 0)
    m |= S_IRUSR;
  else if (readable < 0)
    m &= ~S_IRUSR;

  if(writable > 0)
    m |= S_IWUSR;
  else if (writable < 0)
    m &= ~S_IWUSR;

  if(executable > 0)
    m |= S_IXUSR;
  else if (executable < 0)
    m &= ~S_IXUSR;

// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/chmod-wchmod
  const std::string cpath{path};
#ifdef _MSC_VER
  return _chmod(cpath.c_str(), static_cast<int>(m)) == 0;
#else
  return chmod(cpath.c_str(), m) == 0;
#endif

#endif
}


std::string get_permissions(std::string_view path)
{

  std::string r = "---------";

#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;
  const auto s = Filesystem::status(path, ec);
  if(ec)  FFS_UNLIKELY
  {
    fs_error_callback(path, ec);
    return {};
  }

  const Filesystem::perms p = s.permissions();

#if defined(__cpp_using_enum)  // C++20
  using enum Filesystem::perms;
#else
  constexpr Filesystem::perms none = Filesystem::perms::none;
  constexpr Filesystem::perms owner_read = Filesystem::perms::owner_read;
  constexpr Filesystem::perms owner_write = Filesystem::perms::owner_write;
  constexpr Filesystem::perms owner_exec = Filesystem::perms::owner_exec;
  constexpr Filesystem::perms group_read = Filesystem::perms::group_read;
  constexpr Filesystem::perms group_write = Filesystem::perms::group_write;
  constexpr Filesystem::perms group_exec = Filesystem::perms::group_exec;
  constexpr Filesystem::perms others_read = Filesystem::perms::others_read;
  constexpr Filesystem::perms others_write = Filesystem::perms::others_write;
  constexpr Filesystem::perms others_exec = Filesystem::perms::others_exec;
#endif

  if ((p & owner_read) != none)
    r[0] = 'r';
  if ((p & owner_write) != none)
    r[1] = 'w';
  if ((p & owner_exec) != none)
    r[2] = 'x';

  if(!is_windows){

  if ((p & group_read) != none)
    r[3] = 'r';
  if ((p & group_write) != none)
    r[4] = 'w';
  if ((p & group_exec) != none)
    r[5] = 'x';
  if ((p & others_read) != none)
    r[6] = 'r';
  if ((p & others_write) != none)
    r[7] = 'w';
  if ((p & others_exec) != none)
    r[8] = 'x';

  }

#else

  const mode_t m = fs_st_mode(path);
  if(m == 0)
  {
    fs_error_callback(path);
    return {};
  }

  if (m & S_IRUSR)
    r[0] = 'r';
  if (m & S_IWUSR)
    r[1] = 'w';
  if (m & S_IXUSR)
    r[2] = 'x';

#if !defined(_WIN32)
  if (m & S_IRGRP)
    r[3] = 'r';
  if (m & S_IWGRP)
    r[4] = 'w';
  if (m & S_IXGRP)
    r[5] = 'x';
  if (m & S_IROTH)
    r[6] = 'r';
  if (m & S_IWOTH)
    r[7] = 'w';
  if (m & S_IXOTH)
    r[8] = 'x';
#endif

#endif

  return r;
}

} // namespace ffilesystem
