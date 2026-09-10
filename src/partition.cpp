#include <string>
#include <string_view>

#include <iostream>
#include <system_error>

#include "ffilesystem.h"

#if defined(__linux__) && (__has_include(<linux/magic.h>) || __has_include("linux/magic.h"))
// GCC < 10 doesn't detect <linux/magic.h>
// IWYU pragma: no_include <sys/statfs.h>
#define HAVE_LINUX_MAGIC_H
#include <sys/vfs.h> // IWYU pragma: keep
#include <linux/magic.h>
// https://github.com/torvalds/linux/blob/master/include/uapi/linux/magic.h
#elif defined(FFS_DARWIN) || defined(FFS_BSD)
#include <sys/param.h>
#include <sys/mount.h>
#elif defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif



#ifdef HAVE_LINUX_MAGIC_H
namespace {

#ifdef BTRFS_SUPER_MAGIC
constexpr auto kBtrfsSuperMagic = BTRFS_SUPER_MAGIC;
#else
constexpr unsigned long kBtrfsSuperMagic = 0x9123683EUL;
#endif
#ifdef DEBUGFS_MAGIC
constexpr auto kDebugfsMagic = DEBUGFS_MAGIC;
#else
constexpr unsigned long kDebugfsMagic = 0x64626720UL;
#endif
#ifdef FUSE_SUPER_MAGIC
constexpr auto kFuseSuperMagic = FUSE_SUPER_MAGIC;
#else
constexpr unsigned long kFuseSuperMagic = 0x65735546UL;
#endif
#ifdef EXFAT_SUPER_MAGIC
constexpr auto kExfatSuperMagic = EXFAT_SUPER_MAGIC;
#else
constexpr unsigned long kExfatSuperMagic = 0x2011BAB0UL;
#endif
#ifdef EROFS_SUPER_MAGIC_V1
constexpr auto kErofsSuperMagicV1 = EROFS_SUPER_MAGIC_V1;
#else
constexpr unsigned long kErofsSuperMagicV1 = 0xE0F5E1E2UL;
#endif
#ifdef F2FS_SUPER_MAGIC
constexpr auto kF2fsSuperMagic = F2FS_SUPER_MAGIC;
#else
constexpr unsigned long kF2fsSuperMagic = 0xF2F52010UL;
#endif
#ifdef PROC_SUPER_MAGIC
constexpr auto kProcSuperMagic = PROC_SUPER_MAGIC;
#else
constexpr unsigned long kProcSuperMagic = 0x9FA0UL;
#endif
#ifdef SYSFS_MAGIC
constexpr auto kSysfsMagic = SYSFS_MAGIC;
#else
constexpr unsigned long kSysfsMagic = 0x62656572UL;
#endif
#ifdef TRACEFS_MAGIC
constexpr auto kTracefsMagic = TRACEFS_MAGIC;
#else
constexpr unsigned long kTracefsMagic = 0x74726163UL;
#endif
#ifdef UDF_SUPER_MAGIC
constexpr auto kUdfSuperMagic = UDF_SUPER_MAGIC;
#else
constexpr unsigned long kUdfSuperMagic = 0x15013346UL;
#endif
#ifdef XFS_SUPER_MAGIC
constexpr auto kXfsSuperMagic = XFS_SUPER_MAGIC;
#else
constexpr unsigned long kXfsSuperMagic = 0x58465342UL;
#endif

std::string fs_type_linux(std::string_view path)
{
  struct statfs s;

  if(statfs(std::string{path}.c_str(), &s)) {
    fs_error_callback(path);
    return {};
  }

  switch (s.f_type) {
    case kBtrfsSuperMagic: return "btrfs";
    case kDebugfsMagic: return "debugfs";
    case EXT4_SUPER_MAGIC: return "ext4";
    case kExfatSuperMagic: return "exfat";
    case kErofsSuperMagicV1: return "erofs";
    case kF2fsSuperMagic: return "f2fs";
    case kFuseSuperMagic: return "fuse";
    case NFS_SUPER_MAGIC: return "nfs";
    case kProcSuperMagic: return "procfs";
    case SQUASHFS_MAGIC: return "squashfs";
    case kSysfsMagic: return "sysfs";
    case TMPFS_MAGIC: return "tmpfs";
    case kTracefsMagic: return "tracefs";
    case kUdfSuperMagic: return "udf";
    case V9FS_MAGIC: return "v9fs";
    // used for WSL
    // https://devblogs.microsoft.com/commandline/whats-new-for-wsl-in-windows-10-version-1903/
    case kXfsSuperMagic: return "xfs";

    default:
      fs_error_callback(path,"unknown type ID: " + std::to_string(s.f_type));
      return {};
  }
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
  std::string name(MAX_PATH+1, '\0');

  if(GetVolumeInformationA(r.c_str(), nullptr, 0, nullptr, nullptr, nullptr, name.data(), static_cast<DWORD>(name.size()))) {
    fs_trim(name);
    return name;
  }

#elif defined(__linux__)
# ifdef HAVE_LINUX_MAGIC_H
  return fs_type_linux(path);
# else
  ec = std::make_error_code(std::errc::function_not_supported);
# endif
#elif defined(FFS_DARWIN) || defined(FFS_BSD)
  struct statfs s;

  if(!::statfs(std::string{path}.c_str(), &s))
    return s.f_fstypename;
#else
  ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_error_callback(path, ec);
  return {};
}
