#include <string>
#include <string_view>

#include <iostream>
#include <system_error>

#if defined(__unix__)
// https://github.com/cpredef/predef/blob/master/OperatingSystems.md#bsd-environment
#include <sys/param.h>  // IWYU pragma: keep
#endif

#if defined(__linux__) && (__has_include(<linux/magic.h>) || __has_include("linux/magic.h"))
// GCC < 10 doesn't detect <linux/magic.h>
// IWYU pragma: no_include <sys/statfs.h>
#define HAVE_LINUX_MAGIC_H
#include <sys/vfs.h> // IWYU pragma: keep
#include <linux/magic.h>
// https://github.com/torvalds/linux/blob/master/include/uapi/linux/magic.h
#elif defined(__APPLE__) || defined(BSD)
#include <sys/mount.h>
#elif defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "ffilesystem.h"


#ifdef HAVE_LINUX_MAGIC_H
static inline std::string fs_type_linux(std::string_view path)
{
  struct statfs s;

  if(statfs(path.data(), &s)) {
    fs_print_error(path, "filesystem_type:statfs");
    return {};
  }

  switch (s.f_type) {
    case BTRFS_SUPER_MAGIC: return "btrfs";
    case EXT4_SUPER_MAGIC: return "ext4";
#ifdef EXFAT_SUPER_MAGIC
    case EXFAT_SUPER_MAGIC: return "exfat";
#endif
#ifdef F2FS_SUPER_MAGIC
    case F2FS_SUPER_MAGIC: return "f2fs";
#endif
#ifdef FUSE_SUPER_MAGIC
    case FUSE_SUPER_MAGIC: return "fuse";
#endif
    case NFS_SUPER_MAGIC: return "nfs";
    case SQUASHFS_MAGIC: return "squashfs";
    case TMPFS_MAGIC: return "tmpfs";
#ifdef TRACEFS_MAGIC
    case TRACEFS_MAGIC: return "tracefs";
#endif
#ifdef UDF_SUPER_MAGIC
    case UDF_SUPER_MAGIC: return "udf";
#endif
    case V9FS_MAGIC: return "v9fs";
    // used for WSL
    // https://devblogs.microsoft.com/commandline/whats-new-for-wsl-in-windows-10-version-1903/
#ifdef XFS_SUPER_MAGIC
    case XFS_SUPER_MAGIC: return "xfs";
#endif
    default:
      std::cerr << "ERROR:fs_filesystem_type " << path << " unknown type ID: " << s.f_type << "\n";
      return {};
  }
}
#endif


std::string fs_filesystem_type(std::string_view path)
{
  // return name of filesystem type if known

  std::error_code ec;

#if defined(_WIN32) || defined(__CYGWIN__)

  std::string r(path);

  // Cygwin: assume user input Cygwin path root directly.
  if(!fs_is_cygwin()){

    r = fs_root_name(r);
    if(r.empty())
      return {};

    // GetVolumeInformationA requires a trailing backslash
    r.push_back('\\');
  }

  if(fs_trace) std::cout << "TRACE:filesystem_type(" << path << ") root: " << r << "\n";

  // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getvolumeinformationa
  if(std::string name(MAX_PATH+1, '\0');
      GetVolumeInformationA(r.data(), nullptr, 0, nullptr, nullptr, nullptr, name.data(), static_cast<DWORD>(name.size())))
    return fs_trim(name);
#elif defined(__linux__)
# ifdef HAVE_LINUX_MAGIC_H
  return fs_type_linux(path);
# else
  ec = std::make_error_code(std::errc::function_not_supported);
# endif
#elif defined(__APPLE__) || defined(BSD)
  struct statfs s;
  if(!statfs(path.data(), &s))
    return s.f_fstypename;
#else
  ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_print_error(path, "filesystem_type", ec);
  return {};
}
