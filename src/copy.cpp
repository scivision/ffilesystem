#if defined(__linux__)
#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#if !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE
#endif
#if !defined(_FILE_OFFSET_BITS)
#define _FILE_OFFSET_BITS 64
#endif
#endif

#include "ffilesystem.h"

#include <iostream> // IWYU pragma: keep
#include <system_error>

#include <string_view>

#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Filesystem = std::filesystem;
#else

#include <cstdlib>
#include <memory>  // for std::make_unique

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
// for non-Windows file loop fallback
#include <sys/types.h>  // for off_t, ssize_t
#include <sys/stat.h>
#include <unistd.h> // for read, write
#include <fcntl.h>  // for open, close

// O_CLOEXEC is not defined on all systems. We're not spawning processes,
// so it would only be meaningful possibly if we're invoked multithreaded.
#if !defined(O_CLOEXEC)
#define O_CLOEXEC  0
#endif

#endif

#if defined(__APPLE__) && defined(__MACH__) && __has_include(<copyfile.h>)
// macOS 10.12 or later
#define HAVE_MACOS_COPYFILE
#include <copyfile.h>
#endif

#endif  // HAVE_CXX_FILESYSTEM


#if !defined(HAVE_CXX_FILESYSTEM) && !defined(_WIN32)

static off_t fs_copy_loop(int const rid, int const wid, off_t const len)
{
  // copy a file in chunks
  off_t r = len;

  if (fs_trace) std::cout << "TRACE::ffilesystem:copy_file: using plain file buffer read / write\n";

  constexpr int bufferSize = 16384;
  auto buf = std::make_unique<char[]>(bufferSize);

  ssize_t ret = 0;
  while (r > 0) {
    ret = read(rid, buf.get(), r);
    // value should not be zero because we tell the file size in "len"
    if (ret <= 0 || write(wid, buf.get(), ret) != ret)
      break;

    r -= ret;
  }

  return r;
}


bool fs_copy_file_range_or_loop(std::string_view source, std::string_view dest, bool overwrite)
{
  // copy a file in chunks

#if defined(HAVE_COPY_FILE_RANGE)
  std::string const fst = fs_filesystem_type(source);

  bool useloop = fst == "debugfs" || fst == "procfs" || fst == "sysfs" || fst == "tracefs";
#endif

  int const rid = ::open(source.data(), O_RDONLY | O_CLOEXEC);
  if (rid == -1)
    return false;

  // leave fstat here to avoid source file race condition
  struct stat stat;
  if (fstat(rid, &stat) == -1) {
    ::close(rid);
    return false;
  }

  const off_t len = stat.st_size;

  auto opt = O_CREAT | O_WRONLY | O_TRUNC | O_CLOEXEC;
  if(!overwrite)
    opt |= O_EXCL;

// https://linux.die.net/man/3/open
  int const wid = ::open(dest.data(), opt, stat.st_mode);
  if (wid == -1) {
    ::close(rid);
    return false;
  }

  off_t r = len;
  off_t ret = 0;

#if defined(HAVE_COPY_FILE_RANGE)
    // https://man.freebsd.org/cgi/man.cgi?copy_file_range(2)
    // https://man7.org/linux/man-pages/man2/copy_file_range.2.html
  if (!useloop) {
    if (fs_trace) std::cout << "TRACE::ffilesystem:copy_file: using copy_file_range\n";

    while (r > 0) {
      ret = copy_file_range(rid, nullptr, wid, nullptr, r, 0);
      if (ret <= 0)
        break;

      r -= ret;
    }
  }
#endif

  // https://github.com/boostorg/filesystem/issues/184
  if (r != 0 || (ret < 0 && (errno == EINVAL || errno == EOPNOTSUPP)))
    r = fs_copy_loop(rid, wid, len);

  int const wc = close(wid);
  int const rc = close(rid);

  return wc == 0 && rc == 0 && r == 0;
}
#endif


bool fs_copy_file(std::string_view source, std::string_view dest, bool overwrite)
{
  // copy a single file -- symlinks are followed

  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM
  auto opt = Filesystem::copy_options::none;
  if (overwrite)
    opt = Filesystem::copy_options::overwrite_existing;
// WORKAROUND: Windows MinGW GCC 11..13, Intel oneAPI Linux: bug with overwrite_existing failing on overwrite

  if(overwrite && fs_is_file(dest) && !fs_remove(dest)) FFS_UNLIKELY
    fs_print_error(dest, __func__, std::make_error_code(std::errc::io_error));

  if(Filesystem::copy_file(source, dest, opt, ec) && !ec) FFS_LIKELY
    return true;

#elif defined(_WIN32)
  DWORD opts = 0;
  if(!overwrite)
    opts |= COPY_FILE_FAIL_IF_EXISTS;

  // https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-copyfileexa
  // preserves source file attributes

  if(CopyFileExW(fs_win32_to_wide(source).data(),
                 fs_win32_to_wide(dest).data(),
                 nullptr, nullptr, FALSE, opts) != 0)
    return true;

#elif defined(HAVE_MACOS_COPYFILE)
  // https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/copyfile.3.html
  // preserves source file attributes
  auto opt = COPYFILE_ALL;
  if (!overwrite)
    opt |= COPYFILE_EXCL;

  if(copyfile(source.data(), dest.data(), nullptr, opt) == 0)
    return true;
#else

  if(fs_copy_file_range_or_loop(source, dest, overwrite)) FFS_LIKELY
    return true;

#endif

  fs_print_error(source, dest, __func__, ec);
  return false;

}
