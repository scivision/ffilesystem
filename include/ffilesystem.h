#ifndef FFILESYSTEM_H
#define FFILESYSTEM_H

#ifndef FS_TRACE
#define FS_TRACE 0
#endif

#include <sys/types.h> // mode_t


#ifdef __cplusplus

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

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <algorithm> // std::min
#include <string>
#include <string_view>
#include <ctime> // time_t
#include <optional>
#include <system_error>

#if defined(HAVE_BOOST_FILESYSTEM)
#include <boost/filesystem.hpp>
namespace Fs = boost::filesystem;
namespace Fserr = boost::system;
#elif defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Fs = std::filesystem;
namespace Fserr = std;
std::optional<Fs::file_time_type> fs_get_modtime_fs(std::string_view);
#else
namespace Fserr = std;
#endif


std::string fs_lexically_normal(std::string_view);
std::string fs_make_preferred(std::string_view);

bool fs_is_case_sensitive(std::string_view);
std::string fs_generate_random_alphanumeric_string(const std::string::size_type);

// C++ functions available without C++17 filesystem too
bool fs_slash_first(std::string_view);

std::string fs_with_suffix(std::string_view, std::string_view);

std::string fs_parent(std::string_view);
std::string fs_suffix(std::string_view);

std::string fs_join(std::string_view, std::string_view);

std::string fs_canonical(std::string_view, const bool, const bool);
std::string fs_resolve(std::string_view, const bool, const bool);

bool fs_equivalent(std::string_view, std::string_view);

std::string fs_relative_to(std::string_view, std::string_view);
std::string fs_proximate_to(std::string_view, std::string_view);
bool fs_is_subdir(std::string_view, std::string_view);

std::vector<std::string> fs_split(std::string_view);
std::vector<std::string> fs_normal_vector(std::string_view);
std::string fs_normal(std::string_view);
std::string fs_drop_slash(std::string_view);

std::string::size_type fs_str2char(std::string_view, char*, const std::string::size_type);

void fs_print_error(std::string_view, std::string_view);
void fs_print_error(std::string_view, std::string_view, const std::error_code&);
void fs_print_error(std::string_view, std::string_view, std::string_view, const std::error_code&);


std::string fs_absolute(std::string_view, const bool);
std::string fs_absolute(std::string_view, std::string_view, const bool);

std::string fs_mkdtemp(std::string_view);


bool fs_mkdir(std::string_view);

bool fs_set_modtime(std::string_view);
time_t fs_get_modtime(std::string_view);

std::string fs_which(std::string_view);

bool fs_is_reserved(std::string_view);
bool fs_is_safe_name(std::string_view);

std::string fs_longname(std::string_view);
std::string fs_shortname(std::string_view);

std::string fs_hostname();
std::string fs_cpu_arch();

std::string fs_compiler();

std::string fs_getenv(std::string_view);


std::string fs_get_owner_name(std::string_view);
std::string fs_get_owner_group(std::string_view);


std::string fs_to_cygpath(std::string_view);
std::string fs_to_winpath(std::string_view);

std::string fs_get_locale_name();
std::string fs_get_shell();
std::string fs_get_terminal();
bool fs_setenv(std::string_view, std::string_view);

std::string fs_get_homedir();
std::string fs_get_profile_dir();
std::string fs_user_config_dir();
std::string fs_get_username();

std::string fs_backend();

std::string fs_expanduser(std::string_view);

std::string fs_file_name(std::string_view);

bool fs_touch(std::string_view);

std::string::size_type fs_max_component(std::string_view);

std::string fs_as_posix(std::string_view);
std::string fs_lib_path();
std::string fs_exe_path();

std::string fs_realpath(std::string_view);
std::string fs_trim(std::string r);

std::string fs_stem(std::string_view);

bool fs_is_absolute(std::string_view path);
bool fs_is_symlink(std::string_view);
bool fs_create_symlink(std::string_view, std::string_view);

bool fs_set_permissions(std::string_view, int, int, int);

bool fs_copy_file(std::string_view, std::string_view, bool);

bool fs_exists(std::string_view path);
bool fs_is_file(std::string_view);

bool fs_is_exe(std::string_view);
bool fs_is_empty(std::string_view);
bool fs_is_readable(std::string_view);
bool fs_is_writable(std::string_view);

bool fs_stdin_tty();

std::string fs_root(std::string_view path);
std::string fs_root_name(std::string_view path);

std::uintmax_t fs_space_available(std::string_view);
std::string fs_read_symlink(std::string_view);

std::string fs_get_permissions(std::string_view);

bool fs_is_char_device(std::string_view);
bool fs_is_dir(std::string_view);

std::uintmax_t fs_file_size(std::string_view);
std::uintmax_t fs_hard_link_count(std::string_view);

bool fs_remove(std::string_view);

bool fs_set_cwd(std::string_view);

std::string fs_get_tempdir();
std::string fs_get_cwd();

std::string fs_win32_final_path(std::string_view path);
std::string fs_win32_full_name(std::string_view path);

std::string fs_filesystem_type(std::string_view);

int fs_st_mode(std::string_view path);

// ---------------------------------------------------------------------------

extern "C" {

#else  // C only

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

#ifndef __has_include
#define __has_include(x) 0
#endif

// mostly internal use functions only for C
size_t fs_strncpy(const char*, char*, const size_t);

#endif  // end C-only


#if !defined(__has_c_attribute)
#  define __has_c_attribute(x)  0
#endif

long fs_lang();

size_t fs_get_max_path();
size_t fs_max_component(const char*);

bool fs_is_optimized();

int fs_getpid();

char fs_pathsep();
const char* fs_devnull();

bool fs_is_admin();
bool fs_is_android();
bool fs_is_bsd();
bool fs_is_macos();
bool fs_is_linux();
bool fs_is_unix();
bool fs_is_windows();
int fs_is_wsl();
bool fs_is_mingw();
bool fs_is_cygwin();
bool fs_is_rosetta();

bool fs_has_statx();

bool fs_is_empty(const char*);

bool fs_is_safe_name(const char*);

void fs_as_posix(char*);

size_t fs_normal(const char*, char*, const size_t);

size_t fs_realpath(const char*, char*, const size_t);

size_t fs_join(const char*, const char*, char*, const size_t);
size_t fs_absolute(const char*, const char*, const bool, char*, const size_t);

size_t fs_file_name(const char*, char*, const size_t);
size_t fs_stem(const char*, char*, const size_t);
size_t fs_parent(const char*, char*, const size_t);
size_t fs_suffix(const char*, char*, const size_t);

size_t fs_root(const char*, char*, const size_t);
size_t fs_root_name(const char*, char*, const size_t);

size_t fs_with_suffix(const char*, const char*, char*, const size_t);

size_t fs_which(const char*, char*, const size_t);

bool fs_is_symlink(const char*);
bool fs_create_symlink(const char*, const char*);
size_t fs_read_symlink(const char*, char*, const size_t);

bool fs_mkdir(const char*);
bool fs_exists(const char*);

bool fs_is_absolute(const char*);
bool fs_is_case_sensitive(const char*);
bool fs_is_char_device(const char*);
bool fs_is_dir(const char*);
bool fs_is_file(const char*);
bool fs_is_exe(const char*);
bool fs_is_readable(const char*);
bool fs_is_writable(const char*);
bool fs_is_reserved(const char*);
bool fs_is_subdir(const char*, const char*);

bool fs_set_permissions(const char*, int, int, int);

size_t fs_get_permissions(const char*, char*, const size_t);

bool fs_remove(const char*);

size_t fs_canonical(const char*, const bool, const bool, char*, const size_t);
size_t fs_resolve(const char*, const bool, const bool, char*, const size_t);

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
size_t fs_user_config_dir(char*, const size_t);
size_t fs_get_username(char*, const size_t);

size_t fs_get_owner_name(const char*, char*, const size_t);
size_t fs_get_owner_group(const char*, char*, const size_t);

size_t fs_get_tempdir(char*, const size_t);

size_t fs_expanduser(const char*, char*, const size_t);

uintmax_t fs_file_size(const char*);
uintmax_t fs_space_available(const char*);
uintmax_t fs_hard_link_count(const char*);

size_t fs_backend(char*, const size_t);

size_t fs_exe_path(char*, const size_t);
size_t fs_lib_path(char*, const size_t);

size_t fs_compiler(char*, const size_t);
size_t fs_get_shell(char*, const size_t);
size_t fs_get_terminal(char*, const size_t);

size_t fs_mkdtemp(const char*, char*, const size_t);

size_t fs_hostname(char*, const size_t);

size_t fs_shortname(const char*, char*, const size_t);
size_t fs_longname(const char*, char*, const size_t);

size_t fs_getenv(const char*, char*, const size_t);
bool fs_setenv(const char*, const char*);

size_t fs_filesystem_type(const char*, char*, const size_t);

void fs_print_error(const char*, const char*);

size_t fs_to_cygpath(const char*, char*, const size_t);
size_t fs_to_winpath(const char*, char*, const size_t);

size_t fs_cpu_arch(char*, const size_t);

#ifdef __cplusplus
}
#endif

#endif
