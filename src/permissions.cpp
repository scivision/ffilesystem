#include <string>
#include <string_view>

#include "ffilesystem.h"

#include <system_error>

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#else
#include <sys/types.h>
#include <sys/stat.h> // chmod, permissions constants
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <io.h> // IWYU pragma: keep
// chmod
#endif


bool fs_set_permissions(std::string_view path, int readable, int writable, int executable)
{
// only sets permission for owner, not group or others

#ifdef HAVE_CXX_FILESYSTEM

#if defined(__cpp_using_enum)  // C++20
  using enum std::filesystem::perms;
#else
  constexpr std::filesystem::perms owner_read = std::filesystem::perms::owner_read;
  constexpr std::filesystem::perms owner_write = std::filesystem::perms::owner_write;
  constexpr std::filesystem::perms owner_exec = std::filesystem::perms::owner_exec;
#endif

  std::error_code ec;
  // need to error if path doesn't exist and no operations are requested
  if(!fs_exists(path))
    ec = std::make_error_code(std::errc::no_such_file_or_directory);

  if (!ec && readable != 0)
    std::filesystem::permissions(path, owner_read,
      (readable > 0) ? std::filesystem::perm_options::add : std::filesystem::perm_options::remove,
      ec);

  if (!ec && writable != 0)
    std::filesystem::permissions(path, owner_write,
      (writable > 0) ? std::filesystem::perm_options::add : std::filesystem::perm_options::remove,
      ec);

  if (!ec && executable != 0)
    std::filesystem::permissions(path, owner_exec,
      (executable > 0) ? std::filesystem::perm_options::add : std::filesystem::perm_options::remove,
      ec);

  if(!ec) FFS_LIKELY
    return true;

  fs_print_error(path, __func__, ec);
  return false;

#else

  int m = fs_st_mode(path);
#ifdef _MSC_VER
  const int r = _S_IREAD;
  const int w = _S_IWRITE;
  const int x = _S_IEXEC;
#else
  const int r = S_IRUSR;
  const int w = S_IWUSR;
  const int x = S_IXUSR;
#endif

  if(readable > 0)
    m |= r;
  else if (readable < 0)
    m &= ~r;

  if(writable > 0)
    m |= w;
  else if (writable < 0)
    m &= ~w;

  if(executable > 0)
    m |= x;
  else if (executable < 0)
    m &= ~x;
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/chmod-wchmod
#ifdef _MSC_VER
  return _chmod(path.data(), m) == 0;
#else
  return chmod(path.data(), m) == 0;
#endif

#endif
}


std::string fs_get_permissions(std::string_view path)
{

  std::string r = "---------";

#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;
  const auto s = std::filesystem::status(path, ec);
  if(ec)  FFS_UNLIKELY
  {
    fs_print_error(path, __func__, ec);
    return {};
  }

  const std::filesystem::perms p = s.permissions();

#if defined(__cpp_using_enum)  // C++20
  using enum std::filesystem::perms;
#else
  constexpr std::filesystem::perms none = std::filesystem::perms::none;
  constexpr std::filesystem::perms owner_read = std::filesystem::perms::owner_read;
  constexpr std::filesystem::perms owner_write = std::filesystem::perms::owner_write;
  constexpr std::filesystem::perms owner_exec = std::filesystem::perms::owner_exec;
  constexpr std::filesystem::perms group_read = std::filesystem::perms::group_read;
  constexpr std::filesystem::perms group_write = std::filesystem::perms::group_write;
  constexpr std::filesystem::perms group_exec = std::filesystem::perms::group_exec;
  constexpr std::filesystem::perms others_read = std::filesystem::perms::others_read;
  constexpr std::filesystem::perms others_write = std::filesystem::perms::others_write;
  constexpr std::filesystem::perms others_exec = std::filesystem::perms::others_exec;
#endif

  if ((p & owner_read) != none)
    r[0] = 'r';
  if ((p & owner_write) != none)
    r[1] = 'w';
  if ((p & owner_exec) != none)
    r[2] = 'x';

  if(!fs_is_windows()){

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

  const int m = fs_st_mode(path);
  if(m == 0) FFS_UNLIKELY
  {
    fs_print_error(path, __func__);
    return {};
  }

#if defined(_MSC_VER)
  if (m & _S_IREAD)
    r[0] = 'r';
  if (m & _S_IWRITE)
    r[1] = 'w';
  if (m & _S_IEXEC)
    r[2] = 'x';
#else
  if (m & S_IRUSR)
    r[0] = 'r';
  if (m & S_IWUSR)
    r[1] = 'w';
  if (m & S_IXUSR)
    r[2] = 'x';
#endif

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
