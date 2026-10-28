#ifndef FFILESYSTEM_H
#define FFILESYSTEM_H

#include <sys/types.h> // for pid_t, ino_t, dev_t, mode_t, ...
// https://www.man7.org/linux/man-pages/man3/stat.3type.html


#ifdef _MSC_VER

#ifndef pid_t
#ifdef __cplusplus
using pid_t = long;
#else
typedef long pid_t;
#endif
#endif

#ifndef mode_t
#ifdef __cplusplus
using mode_t = unsigned int;
#else
typedef unsigned int mode_t;
#endif
#endif

#endif // _MSC_VER

#if !defined(__APPLE__) && (defined(BSD) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__))
#define FFS_BSD 1
#endif
#if defined(__APPLE__) && defined(__MACH__)
#define FFS_DARWIN 1
#endif


#ifdef __cplusplus

#include <cstdint> // for uintmax_t

constexpr int fs_trace =
#ifdef FS_TRACE
  1;
#else
  0;
#endif

constexpr std::uintmax_t fs_unknown_size = static_cast<std::uintmax_t>(-1);

#if !defined(__has_cpp_attribute)
// this is a C++20 feature, but available in many older compilers
#  define __has_cpp_attribute(x)  0
#endif

// GCC itself does it this way https://github.com/gcc-mirror/gcc/blob/78b56a12dd028b9b4051422c6bad6260055e4465/libcpp/system.h#L426
#if __has_cpp_attribute(unlikely)
#  define FFS_UNLIKELY [[unlikely]]
#  define FFS_LIKELY [[likely]]
#else
#  define FFS_UNLIKELY
#  define FFS_LIKELY
#endif

#include <cstdlib>
#include <vector>
#include <string>
#include <string_view>
#include <ctime> // for time_t
#include <system_error>
#include <optional>

#if __has_include(<version>)
#include <version>
#endif

#if __has_include(<source_location>)
#include <source_location>
#endif

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#endif

namespace ffilesystem {

constexpr bool is_windows =
#ifdef _WIN32
  true;
#else
  false;
#endif

constexpr char file_separator = (is_windows)
  ? '\\'
  : '/';

constexpr std::string_view file_separators = (is_windows)
  ? "\\/"
  : "/";

constexpr bool is_file_separator(char c) noexcept
{
  return c == '/' || c == file_separator;
}

constexpr char path_separator = (is_windows)
  ? ';'
  : ':';

constexpr std::string_view backend =
#ifdef HAVE_CXX_FILESYSTEM
  "<filesystem>";
#else
  "C";
#endif

constexpr bool has_statx =
// https://www.man7.org/linux/man-pages/man2/statx.2.html
#if defined(HAVE_STATX)
  true;
#else
  false;
#endif

 // C++ version compiler claims to support with given options
constexpr long cpp_lang =
#if defined(_MSVC_LANG) && (_MSVC_LANG > __cplusplus)
  _MSVC_LANG;
#else
  __cplusplus;
#endif

constexpr long cpp_format =
#ifdef __cpp_lib_format
  __cpp_lib_format;
#else
  0;
#endif

constexpr long cpp_ranges =
#if defined(__cpp_lib_ranges) && defined(HAVE_CPP_RANGES)
  __cpp_lib_ranges;
#else
  0;
#endif

constexpr bool is_optimized =
#if defined(__OPTIMIZE__) || defined(FFS_OPTIMIZED)
  true;
#else
  false;
#endif


std::string libcxx();
std::string compiler();

void as_posix(std::string&);
std::string as_posix(std::string_view);

void as_windows(std::string&);
void as_windows(std::wstring&);

std::string absolute(std::string_view);
std::string absolute(std::string_view, std::string_view);


std::string get_homedir();
std::string expanduser(std::string_view);

bool is_absolute(std::string_view);


std::optional<std::string> getenv(std::string_view);
bool setenv(std::string_view, std::string_view);

std::uintmax_t hard_link_count(std::string_view);
std::uintmax_t file_size(std::string_view);
std::uintmax_t space_available(std::string_view);
std::uintmax_t space_capacity(std::string_view);

std::string::size_type max_component(std::string_view);


bool exists(std::string_view);

bool has_root_name(std::string_view);
bool has_root_directory(std::string_view);

bool is_exe(std::string_view);
bool is_executable_binary(std::string_view);

bool is_case_sensitive(std::string_view);
bool is_char_device(std::string_view);
bool is_directory(std::string_view);
bool is_fifo(std::string_view);
bool is_file(std::string_view);
bool is_other(std::string_view);
bool is_reserved(std::string_view);
bool is_wsl();

bool is_readable(std::string_view);
bool is_writable(std::string_view);

bool is_symlink(std::string_view);
bool lexists(std::string_view);


std::size_t get_blksize(std::string_view);

dev_t st_dev(std::string_view);
ino_t inode(std::string_view);

void ascii_lower(std::string&);
void ascii_upper(std::string&);
void trim(std::string&);

bool copy_file(std::string_view, std::string_view, bool);

bool equivalent(std::string_view, std::string_view);

bool touch(std::string_view);

std::string file_name(std::string_view);
bool has_filename(std::string_view);


std::string canonical(std::string_view, const bool = false);
std::string cpu_arch();
std::string exe_path();
std::string filesystem_type(std::string_view);
std::string generate_random_alphanumeric_string(const std::string::size_type);
std::string get_cwd();
std::string get_owner_group(std::string_view);
std::string get_owner_name(std::string_view);
std::string get_permissions(std::string_view);
std::string get_profile_dir();
std::string get_shell();
std::string get_tempdir();
std::string get_terminal();
std::string get_username();
std::string hostname();
std::string lib_path();
std::string longname(std::string_view);
std::string join(std::string_view, std::string_view);
std::string os_version();
std::string parent(std::string_view);
std::string proximate_to(std::string_view, std::string_view);
std::string read_symlink(std::string_view);
std::string realpath(std::string_view);
std::string resolve(std::string_view, const bool = false);
std::string relative_to(std::string_view, std::string_view);
std::string root(std::string_view);
std::string root_name(std::string_view);
std::string root_directory(std::string_view);
std::string shortname(std::string_view);
std::string stem(std::string_view);
std::string suffix(std::string_view);
std::string to_cygpath(std::string_view);
std::string to_winpath(std::string_view);
std::string with_suffix(std::string_view, std::string_view);

std::string drop_slash(std::string_view);
void drop_trailing_slash(std::string&);
std::string normal(std::string_view);

std::string which(std::string_view, std::string_view = {}, const bool = false);


std::time_t get_modtime(std::string_view);
bool set_modtime(std::string_view, const bool = false);

bool create_symlink(std::string_view, std::string_view);
bool stdin_tty();
bool is_admin();
bool is_appexec_alias(std::string_view);
bool is_empty(std::string_view);
bool is_prefix(std::string_view, std::string_view);
bool is_removable(std::string_view);
bool is_rosetta();
bool is_safe_char(const char c);
bool is_safe_name(std::string_view);
bool is_subdir(std::string_view, std::string_view);
bool mkdir(std::string_view);
bool non_ascii(std::string_view);
bool remove(std::string_view);
bool rename(std::string_view, std::string_view);

bool set_cwd(std::string_view);
bool set_permissions(std::string_view, const int, const int, const int);

pid_t getpid();

std::vector<std::string> split_filesep(std::string_view);
std::vector<std::string> split_pathsep(std::string_view);

unsigned long long total_sys_memory();
unsigned long long get_free_memory();

} // namespace ffilesystem


std::string fs_get_locale_name();

std::string fs_lexically_normal(std::string_view);

std::string fs_libc();

std::string fs_make_preferred(std::string_view);

void fs_error_callback(std::string_view
#if defined(__cpp_lib_source_location)
, const std::source_location& = std::source_location::current()
#endif
);

void fs_error_callback(std::string_view, const std::error_code&
#if defined(__cpp_lib_source_location)
, const std::source_location& = std::source_location::current()
#endif
);

void fs_error_callback(std::string_view, std::string_view
#if defined(__cpp_lib_source_location)
, const std::source_location& = std::source_location::current()
#endif
);

void fs_error_callback(std::string_view, std::string_view, const std::error_code&
#if defined(__cpp_lib_source_location)
, const std::source_location& = std::source_location::current()
#endif
);

bool fs_stdin_tty();

std::string::size_type fs_str2char(std::string_view, char*, const std::string::size_type);

// ---------------------------------------------------------------------------

extern "C" {

#else  // C only
#if __STDC_VERSION__ < 202311L
#include <stdbool.h>
#endif

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

// internal use function only for C
size_t fs_strncpy(const char*, char*, const size_t);

#endif  // end C-only


#if !defined(__has_c_attribute)
#  define __has_c_attribute(x)  0
#endif

long fs_cpp_lang();
long fs_cpp_format();
long fs_cpp_ranges();
bool fs_is_optimized();

unsigned long long fs_total_sys_memory();
unsigned long long fs_get_free_memory();

long fs_c_lang();

long libcxx_release();

size_t fs_get_max_path();
long fs_get_max_open_files();
size_t fs_max_component(const char*);


pid_t fs_getpid();

char fs_filesep();
char fs_pathsep();
const char* fs_devnull();

bool fs_is_admin();
bool fs_is_android();
bool fs_is_bsd();
bool fs_is_macos();
bool fs_is_linux();
bool fs_is_unix();
bool fs_is_windows();
bool fs_is_wsl();
bool fs_is_mingw();
bool fs_is_clang();
bool fs_is_clangcl();
bool fs_is_msvc();
bool fs_is_appleclang();

bool fs_win32_long_paths_enabled();
bool fs_is_cygwin();
bool fs_is_rosetta();

bool fs_has_statx();

bool fs_is_empty(const char*);

bool fs_is_safe_name(const char*);
bool fs_is_appexec_alias(const char*);

size_t fs_as_posix(char*, const size_t);
size_t fs_as_windows(char*, const size_t);

size_t fs_normal(const char*, char*, const size_t);

size_t fs_realpath(const char*, char*, const size_t);

size_t fs_join(const char*, const char*, char*, const size_t);
size_t fs_absolute(const char*, const char*, char*, const size_t);

size_t fs_file_name(const char*, char*, const size_t);
size_t fs_stem(const char*, char*, const size_t);
size_t fs_parent(const char*, char*, const size_t);
size_t fs_suffix(const char*, char*, const size_t);

size_t fs_root(const char*, char*, const size_t);

size_t fs_root_name(const char*, char*, const size_t);
bool fs_has_root_name(const char*);

size_t fs_root_directory(const char*, char*, const size_t);
bool fs_has_root_directory(const char*);

size_t fs_with_suffix(const char*, const char*, char*, const size_t);

size_t fs_which(const char*, const char*, const bool, char*, const size_t);

bool fs_is_symlink(const char*);
bool fs_create_symlink(const char*, const char*);
size_t fs_read_symlink(const char*, char*, const size_t);

bool fs_mkdir(const char*);

bool fs_exists(const char*);
bool fs_lexists(const char*);
bool fs_is_removable(const char*);

bool fs_has_filename(const char*);
bool fs_is_absolute(const char*);
bool fs_is_case_sensitive(const char*);

bool fs_is_char_device(const char*);
bool fs_is_fifo(const char*);

bool fs_is_dir(const char*);
bool fs_is_file(const char*);

bool fs_is_other(const char*);

bool fs_is_exe(const char*);
bool fs_is_executable_binary(const char*);

bool fs_is_readable(const char*);
bool fs_is_writable(const char*);
bool fs_is_reserved(const char*);

bool fs_is_prefix(const char*, const char*);
bool fs_is_subdir(const char*, const char*);

bool fs_set_permissions(const char*, const int, const int, const int);

size_t fs_get_permissions(const char*, char*, const size_t);

bool fs_remove(const char*);
bool fs_rename(const char*, const char*);

size_t fs_canonical(const char*, const bool, char*, const size_t);
size_t fs_resolve(const char*, const bool, char*, const size_t);

bool fs_equivalent(const char*, const char*);
bool fs_copy_file(const char*, const char*, bool);

size_t fs_relative_to(const char*, const char*, char*, const size_t);
size_t fs_proximate_to(const char*, const char*, char*, const size_t);

bool fs_touch(const char*);

time_t fs_get_modtime(const char*);
bool fs_set_modtime(const char*);

size_t fs_get_cwd(char*, const size_t);
bool fs_set_cwd(const char*);

size_t fs_get_homedir(char*, const size_t);
size_t fs_get_profile_dir(char*, const size_t);
size_t fs_get_username(char*, const size_t);

size_t fs_get_owner_name(const char*, char*, const size_t);
size_t fs_get_owner_group(const char*, char*, const size_t);

size_t fs_get_tempdir(char*, const size_t);

size_t fs_expanduser(const char*, char*, const size_t);

uintmax_t fs_file_size(const char*);

uintmax_t fs_space_available(const char*);
uintmax_t fs_space_capacity(const char*);
size_t fs_get_blksize(const char*);

ino_t fs_inode(const char*);
dev_t fs_st_dev(const char*);

uintmax_t fs_hard_link_count(const char*);

size_t fs_backend(char*, const size_t);

size_t fs_exe_path(char*, const size_t);
size_t fs_lib_path(char*, const size_t);

size_t fs_compiler(char*, const size_t);
size_t fs_get_shell(char*, const size_t);
size_t fs_get_terminal(char*, const size_t);

size_t fs_hostname(char*, const size_t);

size_t fs_shortname(const char*, char*, const size_t);
size_t fs_longname(const char*, char*, const size_t);

size_t fs_getenv(const char*, char*, const size_t);
bool fs_setenv(const char*, const char*);

size_t fs_filesystem_type(const char*, char*, const size_t);

typedef void (*fs_error_callback_t)(const char*);

// Set to NULL to suppress diagnostics. The default callback writes to stderr.
void fs_set_error_callback(fs_error_callback_t);
void fs_reset_error_callback();

size_t fs_to_cygpath(const char*, char*, const size_t);
size_t fs_to_winpath(const char*, char*, const size_t);

size_t fs_cpu_arch(char*, const size_t);

#ifdef __cplusplus
}
#endif

#endif
