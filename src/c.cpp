// interfaces for C

#include <cstddef> // size_t
#include <cstdint> // uintmax_t

#include <iostream>
#include <system_error>

#include <cstring>

#include <string>
#include <string_view>

#if defined(HAVE_ISO_FORTRAN_BINDING)
#include <ISO_Fortran_binding.h>
#endif

#include "ffilesystem.h"


std::string::size_type fs_str2char(std::string_view s, char* result, const std::string::size_type buffer_size)
{
  if(s.length() >= buffer_size) FFS_UNLIKELY
  {
    if(fs_trace) std::cerr << "ERROR:Ffs:str2char(" << s << ") output buffer " << buffer_size << " too small for string: " << s << " length " << s.length() << "\n";

    fs_error_callback(s, std::make_error_code(std::errc::result_out_of_range));
    return 0;
  }

  s.copy(result, buffer_size);
  result[s.length()] = '\0';
  return s.length();
}

pid_t fs_getpid() { return ffilesystem::getpid(); }
bool fs_is_admin() { return ffilesystem::is_admin(); }
bool fs_stdin_tty() { return ffilesystem::stdin_tty(); }

bool fs_is_optimized() { return ffilesystem::is_optimized; }
long fs_cpp_lang() { return ffilesystem::cpp_lang; }
long fs_cpp_format() { return ffilesystem::cpp_format; }
long fs_cpp_ranges() { return ffilesystem::cpp_ranges; }

bool fs_has_statx() { return ffilesystem::has_statx; }
bool fs_is_rosetta() { return ffilesystem::is_rosetta(); }
bool fs_is_windows() { return ffilesystem::is_windows; }

char fs_filesep(){ return ffilesystem::file_separator; }
char fs_pathsep(){ return ffilesystem::path_separator; }

unsigned long long fs_total_sys_memory(){ return ffilesystem::total_sys_memory(); }
unsigned long long fs_get_free_memory(){ return ffilesystem::get_free_memory(); }

std::string::size_type fs_absolute(const char* path, const char* base,
                          char* out, const std::string::size_type buffer_size){
  return fs_str2char(ffilesystem::absolute(path, base), out, buffer_size);
}

std::string::size_type fs_parent(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(ffilesystem::parent(path), result, buffer_size);
}


std::string::size_type fs_suffix(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(ffilesystem::suffix(path), result, buffer_size);
}

std::string::size_type fs_join(const char* path, const char* other,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(ffilesystem::join(path, other), result, buffer_size);
}

std::string::size_type fs_normal(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(ffilesystem::normal(path), result, buffer_size);
}

std::string::size_type fs_canonical(const char* path, const bool strict,
                          char* result, const std::string::size_type buffer_size){
  return fs_str2char(ffilesystem::canonical(path, strict), result, buffer_size);
}

std::string::size_type fs_resolve(const char* path, const bool strict,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(ffilesystem::resolve(path, strict), result, buffer_size);
}


bool fs_equivalent(const char* path1, const char* path2){ return ffilesystem::equivalent(path1, path2); }

std::string::size_type fs_relative_to(const char* base, const char* other,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(ffilesystem::relative_to(base, other), result, buffer_size);
}

std::string::size_type fs_proximate_to(const char* base, const char* other,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(ffilesystem::proximate_to(base, other), result, buffer_size);
}

bool fs_is_prefix(const char* prefix, const char* path){ return ffilesystem::is_prefix(prefix, path); }

bool fs_is_subdir(const char* subdir, const char* dir){ return ffilesystem::is_subdir(subdir, dir); }


bool fs_mkdir(const char* path){ return ffilesystem::mkdir(path); }

size_t fs_longname(const char* in, char* result, const size_t buffer_size){ return fs_str2char(ffilesystem::longname(in), result, buffer_size); }
size_t fs_shortname(const char* in, char* result, const size_t buffer_size){ return fs_str2char(ffilesystem::shortname(in), result, buffer_size); }

bool fs_copy_file(const char* source, const char* dest, bool overwrite){ return ffilesystem::copy_file(source, dest, overwrite); }

size_t fs_hostname(char* name, const size_t buffer_size) { return fs_str2char(ffilesystem::hostname(), name, buffer_size); }

size_t fs_cpu_arch(char* arch, const size_t buffer_size) { return fs_str2char(ffilesystem::cpu_arch(), arch, buffer_size); }

size_t fs_get_terminal(char* name, const size_t buffer_size) { return fs_str2char(ffilesystem::get_terminal(), name, buffer_size); }

size_t fs_get_shell(char* name, const size_t buffer_size) { return fs_str2char(ffilesystem::get_shell(), name, buffer_size); }


size_t fs_getenv(const char* name, char* value, const size_t buffer_size)
{
  return fs_str2char(ffilesystem::getenv(name).value_or(""), value, buffer_size);
}

bool fs_setenv(const char* name, const char* value){ return ffilesystem::setenv(name, value); }

size_t fs_get_homedir(char* path, const size_t buffer_size)
{
  return fs_str2char(ffilesystem::get_homedir(), path, buffer_size);
}

size_t fs_get_profile_dir(char* path, const size_t buffer_size) { return fs_str2char(ffilesystem::get_profile_dir(), path, buffer_size); }

size_t fs_get_username(char* name, const size_t buffer_size) { return fs_str2char(ffilesystem::get_username(), name, buffer_size); }

size_t fs_expanduser(const char* path, char* result, const size_t buffer_size)
{
  return fs_str2char(ffilesystem::expanduser(path), result, buffer_size);
}

bool fs_touch(const char* path){ return ffilesystem::touch(path); }

std::string::size_type fs_compiler(char* name, const size_t buffer_size){ return fs_str2char(ffilesystem::compiler(), name, buffer_size); }

std::string::size_type fs_max_component(const char* path){ return ffilesystem::max_component(path); }

std::size_t fs_exe_path(char* path, const size_t buffer_size){ return fs_str2char(ffilesystem::exe_path(), path, buffer_size); }
std::size_t fs_lib_path(char* path, const size_t buffer_size){ return fs_str2char(ffilesystem::lib_path(), path, buffer_size); }

std::size_t fs_backend(char* r, const size_t buffer_size){ return fs_str2char(ffilesystem::backend, r, buffer_size); }

std::size_t fs_as_posix(const char* path, char* result, const size_t buffer_size)
{
  return fs_str2char(ffilesystem::as_posix(path), result, buffer_size);
}

std::size_t fs_as_windows(char* path, const size_t buffer_size)
{
  std::string s{path};
  ffilesystem::as_windows(s);
  return fs_str2char(s, path, buffer_size);
}


std::size_t fs_realpath(const char* path, char* result, const size_t buffer_size)
{
  return fs_str2char(ffilesystem::realpath(path), result, buffer_size);
}

#if defined(HAVE_ISO_FORTRAN_BINDING)
extern "C" {
int realpath_cfi(const CFI_cdesc_t *in_desc, CFI_cdesc_t *out_desc) {
    if(!in_desc || !out_desc || !in_desc->base_addr || !out_desc->base_addr)
      return -1;

    if(in_desc->rank != 0 || out_desc->rank != 0)
      return -2;

    if(in_desc->type != CFI_type_char || out_desc->type != CFI_type_char)
      return -3;

    std::string::size_type Lin = in_desc->elem_len;
    const std::string::size_type Lout = out_desc->elem_len;
    if(Lout == 0)
      return -4;

    const char *in_ptr = static_cast<const char *>(in_desc->base_addr);
    char *out_ptr = static_cast<char *>(out_desc->base_addr);

    // Trim trailing spaces from the input string, if any
    while(Lin > 0 && in_ptr[Lin - 1] == ' ')
      --Lin;

    std::memset(out_ptr, ' ', Lout);

    if(Lin == 0)
      return 0;

    // Fortran character data is fixed-width and not null-terminated.
    const std::string in_value(in_ptr, Lin);
    const std::string out_value = ffilesystem::realpath(in_value);
    if(out_value.empty())
      return 0;

    if(out_value.size() > Lout)
      return -5;

    out_value.copy(out_ptr, out_value.size());

    return static_cast<int>(out_value.size());
}
}
#endif



size_t fs_root(const char* path, char* result, const size_t buffer_size)
{
  return fs_str2char(ffilesystem::root(path), result, buffer_size);
}

size_t fs_root_name(const char* path, char* result, const size_t buffer_size)
{
  return fs_str2char(ffilesystem::root_name(path), result, buffer_size);
}

size_t fs_root_directory(const char* path, char* result, const size_t buffer_size)
{
  return fs_str2char(ffilesystem::root_directory(path), result, buffer_size);
}

bool fs_has_root_directory(const char* path) { return ffilesystem::has_root_directory(path); }

bool fs_has_root_name(const char* path) { return ffilesystem::has_root_name(path); }

size_t fs_get_owner_name(const char* path, char* result, const size_t buffer_size)
{
  return fs_str2char(ffilesystem::get_owner_name(path), result, buffer_size);
}

size_t fs_get_owner_group(const char* path, char* result, const size_t buffer_size)
{
  return fs_str2char(ffilesystem::get_owner_group(path), result, buffer_size);
}

std::string::size_type fs_stem(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(ffilesystem::stem(path), result, buffer_size);
}

std::string::size_type fs_get_cwd(char* path, const std::string::size_type buffer_size){
  return fs_str2char(ffilesystem::get_cwd(), path, buffer_size);
}

bool fs_set_cwd(const char* path){ return ffilesystem::set_cwd(path); }

std::string::size_type fs_get_tempdir(char* path, const std::string::size_type buffer_size){
  return fs_str2char(ffilesystem::get_tempdir(), path, buffer_size);
}

bool fs_has_filename(const char* path) { return ffilesystem::has_filename(path); }

bool fs_is_absolute(const char* path) { return ffilesystem::is_absolute(path); }

bool fs_is_empty(const char* path){ return ffilesystem::is_empty(path); }

std::uintmax_t fs_space_available(const char* path){ return ffilesystem::space_available(path); }

std::uintmax_t fs_space_capacity(const char* path){ return ffilesystem::space_capacity(path); }

std::size_t fs_get_blksize(const char* path){ return ffilesystem::get_blksize(path); }

ino_t fs_inode (const char* path){ return ffilesystem::inode(path); };
dev_t fs_st_dev(const char* path){ return ffilesystem::st_dev(path); };


std::uintmax_t fs_hard_link_count(const char* path){ return ffilesystem::hard_link_count(path); }

bool fs_set_permissions(const char* path, int readable, int writable, int executable){
  return ffilesystem::set_permissions(path, readable, writable, executable);
}

std::string::size_type fs_file_name(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(ffilesystem::file_name(path), result, buffer_size);
}

std::string::size_type fs_filesystem_type(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(ffilesystem::filesystem_type(path), result, buffer_size);
}

time_t fs_get_modtime(const char* path){ return ffilesystem::get_modtime(path); }

bool fs_set_modtime(const char* path){ return ffilesystem::set_modtime(path); }

bool fs_is_reserved(const char* path){ return ffilesystem::is_reserved(path); }

bool fs_is_safe_name(const char* path){ return ffilesystem::is_safe_name(path); }

bool fs_remove(const char* path){ return ffilesystem::remove(path); }

bool fs_rename(const char* from, const char* to){ return ffilesystem::rename(from, to); }

bool fs_is_appexec_alias(const char* path){ return ffilesystem::is_appexec_alias(path); }

bool fs_is_case_sensitive(const char* path) { return ffilesystem::is_case_sensitive(path); }

bool fs_is_char_device(const char* path){ return ffilesystem::is_char_device(path); }

bool fs_is_fifo(const char* path){ return ffilesystem::is_fifo(path); }

bool fs_is_dir(const char* path){ return ffilesystem::is_directory(path); }

bool fs_is_exe(const char* path){ return ffilesystem::is_exe(path); }

bool fs_is_executable_binary(const char* path){ return ffilesystem::is_executable_binary(path); }

bool fs_exists(const char* path){ return ffilesystem::exists(path); }

bool fs_lexists(const char* path){ return ffilesystem::lexists(path); }

bool fs_is_file(const char* path){ return ffilesystem::is_file(path); }

bool fs_is_other(const char* path){ return ffilesystem::is_other(path); }

bool fs_is_readable(const char* path){ return ffilesystem::is_readable(path); }

bool fs_is_removable(const char* path){ return ffilesystem::is_removable(path); }

bool fs_is_writable(const char* path){ return ffilesystem::is_writable(path); }

bool fs_is_wsl(){ return ffilesystem::is_wsl(); }

bool fs_create_symlink(const char* target, const char* link){ return ffilesystem::create_symlink(target, link); }

bool fs_is_symlink(const char* path){ return ffilesystem::is_symlink(path); }

std::string::size_type fs_read_symlink(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(ffilesystem::read_symlink(path), result, buffer_size);
}

std::string::size_type fs_get_permissions(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(ffilesystem::get_permissions(path), result, buffer_size);
}

uintmax_t fs_file_size(const char* path){ return ffilesystem::file_size(path); }

size_t fs_to_cygpath(const char* path, char* result, const size_t buffer_size){
  return fs_str2char(ffilesystem::to_cygpath(path), result, buffer_size);
}

size_t fs_to_winpath(const char* path, char* result, const size_t buffer_size){
  return fs_str2char(ffilesystem::to_winpath(path), result, buffer_size);
}


size_t fs_which(const char* name, const char* path, const bool find_all, char* result, const size_t buffer_size){
  return fs_str2char(ffilesystem::which(name, path, find_all), result, buffer_size);
}

std::string::size_type fs_with_suffix(const char* path, const char* new_suffix,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(ffilesystem::with_suffix(path, new_suffix), result, buffer_size);
}

#if !defined(ffilesystem_extra)

namespace ffilesystem {

std::string exe_path(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return {};
}

std::string filesystem_type(std::string_view path){
  fs_error_callback(path, std::make_error_code(std::errc::function_not_supported));
  return {};
}

std::string generate_random_alphanumeric_string(const std::string::size_type){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return {};
}

std::string lib_path(){
  fs_error_callback("", std::make_error_code(std::errc::function_not_supported));
  return {};
}

std::string::size_type max_component(std::string_view path){
  fs_error_callback(path, std::make_error_code(std::errc::function_not_supported));
  return 0;
}

bool is_case_sensitive(std::string_view path){
  fs_error_callback(path, std::make_error_code(std::errc::function_not_supported));
  return false;
}

bool is_removable(std::string_view path){
  fs_error_callback(path, std::make_error_code(std::errc::function_not_supported));
  return false;
}

}

#endif
