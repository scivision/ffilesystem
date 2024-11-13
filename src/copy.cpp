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

#include <string>  // IWYU pragma: keep
#include <string_view>

#if defined(HAVE_BOOST_FILESYSTEM)
#include <boost/filesystem.hpp>
#elif defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
#else
#include <cstdio>  // fopen, fclose, fread, fwrite
#include <cstdlib>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(FFS_MACOS)
#include <copyfile.h>
#elif defined(__linux__) || defined(BSD)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#endif
#endif


bool fs_copy_file(std::string_view source, std::string_view dest, bool overwrite)
{
#if defined(HAVE_CXX_FILESYSTEM) || defined(HAVE_BOOST_FILESYSTEM)
  auto opt = Fs::copy_options::none;
  if (overwrite)
    opt = Fs::copy_options::overwrite_existing;
// WORKAROUND: Windows MinGW GCC 11..13, Intel oneAPI Linux: bug with overwrite_existing failing on overwrite

  if(overwrite && fs_is_file(dest) && !fs_remove(dest)) FFS_UNLIKELY
    fs_print_error(dest, "copy_file:remove");

  Fserr::error_code ec;
  if(Fs::copy_file(source, dest, opt, ec) && !ec) FFS_LIKELY
    return true;

  fs_print_error(source, "copy_file", ec);
  return false;
#else

  if(overwrite && fs_is_file(dest) && !fs_remove(dest))
    fs_print_error(dest, "copy_file:remove");

#if defined(_WIN32)
    if(!CopyFileA(source.data(), dest.data(), true)){
      fs_print_error(source, "copy_file:CopyFile");
      return false;
    }
#elif defined(FFS_MACOS)
  /* copy-on-write file
  * based on kwSys:SystemTools:CloneFileContent
  * https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/copyfile.3.html
  * COPYFILE_CLONE is a 'best try' flag, which falls back to a copy if the clone fails.
  * These flags are meant to be COPYFILE_METADATA | COPYFILE_CLONE, but CLONE
  * forces COPYFILE_NOFOLLOW_SRC and that violates the invariant that this
  * should result in a file.
  * https://gitlab.kitware.com/utils/kwsys/-/commit/ee3223d7ae9a5b52b0a30efb932436def80c0d92
  */
  if(copyfile(source.data(), dest.data(), nullptr,
      COPYFILE_METADATA | COPYFILE_EXCL | COPYFILE_STAT | COPYFILE_XATTR | COPYFILE_DATA) < 0){
    fs_print_error(source, "copy_file:copyfile");
    return false;
  }
#elif defined(HAVE_COPY_FILE_RANGE)
    // use copy_file_range
    // https://man.freebsd.org/cgi/man.cgi?copy_file_range(2)
    // https://man7.org/linux/man-pages/man2/copy_file_range.2.html
    // https://linux.die.net/man/3/open

if (FS_TRACE) std::cout << "TRACE::ffilesystem:copy_file: using copy_file_range\n";

  const int rid = open(source.data(), O_RDONLY);
  if (rid == -1) {
    fs_print_error(source, "copy_file:open");
    return false;
  }

  // leave fstat here to avoid source file race conditino
  struct stat  stat;
  if (fstat(rid, &stat) == -1) {
    fs_print_error(source, "copy_file:fstat");
    close(rid);
    return false;
  }

  off_t len = stat.st_size;

  const int wid = open(dest.data(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
  if (wid == -1) {
    fs_print_error(dest, "copy_file:open");
    close(rid);
    return false;
  }

  off_t ret;

  do {
    ret = copy_file_range(rid, nullptr, wid, nullptr, len, 0);
    if (ret == -1) {
        fs_print_error(dest, "copy_file:copy_file_range");
        close(rid);
        close(wid);
        return false;
    }

    len -= ret;
  } while (len > 0 && ret > 0);

  close(rid);
  close(wid);

#else
    // https://stackoverflow.com/a/29082484

if (FS_TRACE) std::cout << "TRACE::ffilesystem:copy_file: using fallback fread/fwrite\n";
  const int bufferSize = 4096;
  std::string buf(bufferSize, '\0');

  FILE *rid = fopen(source.data(), "r");
  if (!rid) {
    fs_print_error(source, "copy_file:fopen");
    return false;
  }

  FILE *wid = fopen(dest.data(), "w");
  if (!wid) {
    fs_print_error(dest, "copy_file:fopen");
    fclose(rid);
    return false;
  }

  while (!feof(rid)) {
    size_t bytes = fread(buf.data(), 1, bufferSize, rid);
    if (bytes)
      fwrite(buf.data(), 1, bytes, wid);
  }

  fclose(rid);
  fclose(wid);
#endif

  return fs_is_file(dest);
#endif
}
