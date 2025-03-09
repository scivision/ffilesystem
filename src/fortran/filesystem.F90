module filesystem

use, intrinsic:: iso_c_binding, only: C_BOOL, C_CHAR, C_INT, C_LONG, C_LONG_LONG, C_SIZE_T, C_NULL_CHAR, C_DOUBLE
use, intrinsic:: iso_fortran_env, only: int64, compiler_version, stderr=>error_unit

implicit none
private
!! utility procedures
public :: get_homedir, get_profile_dir, user_config_dir, get_username, hostname, &
 get_owner_name, get_owner_group, &
 canonical, resolve, realpath, fs_getpid, &
 get_cwd, set_cwd, which
public :: normal, expanduser, as_posix, &
is_absolute, is_char_device, is_fifo, is_case_sensitive, is_dir, is_file, &
is_exe, is_executable_binary, is_other, &
is_prefix, is_subdir, &
is_appexec_alias, &
is_readable, is_writable, is_reserved, &
is_empty, &
is_symlink, read_symlink, create_symlink, &
is_removable, exists, &
join, &
copy_file, mkdir, &
relative_to, proximate_to, &
hard_link_count, &
root, root_name, same_file, file_size, &
space_available, space_capacity, &
file_name, parent, stem, suffix, with_suffix, &
absolute, &
assert_is_file, assert_is_dir, &
touch, get_modtime, set_modtime, &
remove, fs_rename, &
get_tempdir, &
set_permissions, get_permissions, &
backend, cpp_lang, c_lang, &
fs_lang, &
fs_is_optimized, pathsep, is_safe_name, &
is_admin, is_bsd, is_macos, is_rosetta, is_windows, is_cygwin, is_wsl, is_mingw, is_msvc, is_linux, is_unix, &
max_path, max_component, &
exe_path, lib_path, compiler, compiler_c, get_shell, get_terminal, &
longname, shortname, getenv, setenv, getarg, &
is_alpha, filesystem_type, devnull, cpu_arch, &
to_cygpath, to_winpath, &
stdin_tty


!> legacy function name make_absolute() is actually absolute()
interface make_absolute
  module procedure absolute
end interface


interface

logical(C_BOOL) function fs_is_optimized() bind(C)
!! ffilesystem is optimized for speed?
import C_BOOL
end function

logical(C_BOOL) function stdin_tty() bind(C)
!! stdin is a terminal
import C_BOOL
end function

integer(C_LONG) function fs_lang() bind(C, name="fs_cpp_lang")
!! deprecated: use `cpp_lang` instead
import C_LONG
end function

integer(C_LONG) function c_lang() bind(C, name="fs_c_lang")
!! C level of macro `__STDC_VERSION__`
import C_LONG
end function

integer(C_LONG) function cpp_lang() bind(C, name="fs_cpp_lang")
!! C++ level of macro `__cplusplus`
import C_LONG
end function

logical(C_BOOL) function is_admin() bind(C, name="fs_is_admin")
!! user running as admin / root / superuser?
import
end function

logical(C_BOOL) function is_bsd() bind(C, name="fs_is_bsd")
!! operating system is BSD-like
import
end function

logical(C_BOOL) function is_macos() bind(C, name="fs_is_macos")
!! operating system is macOS
import
end function

logical(C_BOOL) function is_rosetta() bind(C, name="fs_is_rosetta")
!! running on Apple Silicon with Rosetta 2
import
end function

logical(C_BOOL) function is_windows() bind(C, name="fs_is_windows")
!! operating system is Microsoft Windows
import
end function

logical(C_BOOL) function is_cygwin() bind(C, name="fs_is_cygwin")
!! operating system is Cygwin
import
end function

integer(C_INT) function is_wsl() bind(C, name="fs_is_wsl")
!! Windows Subsystem for Linux (WSL) version (0 is not WSL)
import
end function

integer(C_INT) function fs_getpid() bind(C)
!! process ID
import
end function

logical(C_BOOL) function is_mingw() bind(C, name="fs_is_mingw")
!! operating system platform is MinGW
import C_BOOL
end function

logical(C_BOOL) function is_msvc() bind(C, name="fs_is_msvc")
!! compiler is MSVC-like (including oneAPI on Windows with MSVC frontend)
import C_BOOL
end function


logical(C_BOOL) function is_linux() bind(C, name="fs_is_linux")
!! operating system is Linux
import C_BOOL
end function

logical(C_BOOL) function is_unix() bind(C, name="fs_is_unix")
!! operating system is Unix-like
import C_BOOL
end function

integer(C_SIZE_T) function fs_get_max_path() bind(c)
import
end function

integer(C_SIZE_T) function fs_max_component(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

integer (C_SIZE_T) function fs_as_posix(path, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(C_BOOL) function fs_is_empty(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

integer(C_SIZE_T) function fs_canonical(path, strict, expand_tilde, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
logical(C_BOOL), intent(in), value :: strict, expand_tilde
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_realpath(path, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_resolve(path, strict, expand_tilde, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
logical(C_BOOL), intent(in), value :: strict, expand_tilde
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(C_BOOL) function fs_set_permissions(path, readable, writable, executable) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
integer(C_INT), intent(in), value :: readable, writable, executable
end function

integer(C_SIZE_T) function fs_get_permissions(path, perms, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: perms(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(C_BOOL) function fs_copy_file(source, dest, overwrite) bind(C)
import
character(kind=c_char), intent(in) :: source(*), dest(*)
logical(C_BOOL), intent(in), value :: overwrite
end function


logical(C_BOOL) function fs_equivalent(path1, path2) bind(C)
import C_BOOL, C_CHAR
character(kind=C_CHAR), intent(in) :: path1(*), path2(*)
end function

logical(C_BOOL) function fs_is_symlink(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

integer(C_SIZE_T) function fs_compiler(path, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(out) :: path(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_get_shell(path, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(out) :: path(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_get_terminal(path, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(out) :: path(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_read_symlink(path, result, buffer_size) bind(C)
import
character(kind=c_char), intent(in) :: path(*)
character(kind=c_char), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(C_BOOL) function fs_create_symlink(target, link) bind(C)
import
character(kind=C_CHAR), intent(in) :: target(*), link(*)
end function

logical(C_BOOL) function fs_mkdir(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(C_BOOL) function fs_remove(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical (C_BOOL) function fs_rename_c(oldpath, newpath) bind(C, name="fs_rename")
import
character(kind=C_CHAR), intent(in) :: oldpath(*), newpath(*)
end function

logical(C_BOOL) function fs_exists(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(C_BOOL) function fs_is_removable(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

integer(C_SIZE_T) function fs_expanduser(path, result, buffer_size) bind(C)
import
character(kind=c_char), intent(in) :: path(*)
character(kind=c_char), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_file_name(path, filename, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: filename(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(C_BOOL) function fs_is_safe_name(filename) bind(C)
import
character(kind=C_CHAR), intent(in) :: filename(*)
end function

integer(C_SIZE_T) function fs_file_size(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

integer(C_SIZE_T) function fs_hard_link_count(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

integer(C_SIZE_T) function fs_space_available(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

integer (C_SIZE_T) function fs_space_capacity(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

integer(C_SIZE_T) function fs_get_cwd(path, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(out) :: path(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(C_BOOL) function fs_set_cwd(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

integer (C_SIZE_T) function fs_hostname(name, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(out) :: name(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer (C_SIZE_T) function fs_get_username(name, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(out) :: name(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer (C_SIZE_T) function fs_get_owner_name(path, name, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: name(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer (C_SIZE_T) function fs_get_owner_group(path, name, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: name(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer (C_SIZE_T) function fs_user_config_dir(path, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(out) :: path(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_get_homedir(path, buffer_size) bind(C)
import
character(kind=c_char), intent(out) :: path(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_get_profile_dir(path, buffer_size) bind(C)
import
character(kind=c_char), intent(out) :: path(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_get_tempdir(path, buffer_size) bind(C)
import
character(kind=c_char), intent(out) :: path(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(c_bool) function fs_is_absolute(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(C_BOOL) function fs_is_appexec_alias(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(C_BOOL) function fs_is_char_device(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(C_BOOL) function fs_is_fifo(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(C_BOOL) function fs_is_file(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(C_BOOL) function fs_is_other(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(C_BOOL) function fs_is_case_sensitive(path) bind(C)
!! filesystem is case sensitive
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(C_BOOL) function fs_is_dir(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(c_bool) function fs_is_exe(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(C_BOOL) function fs_is_executable_binary(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(c_bool) function fs_is_readable(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(c_bool) function fs_is_writable(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(c_bool) function fs_is_reserved(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

integer(C_SIZE_T) function fs_join(path, other, result, buffer_size) bind(C)
import
character(kind=c_char), intent(in) :: path(*), other(*)
character(kind=c_char), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(C_BOOL) function fs_is_prefix(prefix, path) bind(C)
import
character(kind=C_CHAR), intent(in) :: prefix(*), path(*)
end function

logical(C_BOOL) function fs_is_subdir(subdir, dir) bind(C)
import
character(kind=C_CHAR), intent(in) :: subdir(*), dir(*)
end function

integer(C_SIZE_T) function fs_absolute(path, base, expand_tilde, result, buffer_size) bind(C)
import
character(kind=c_char), intent(in) :: path(*), base(*)
logical(C_BOOL), intent(in), value :: expand_tilde
character(kind=c_char), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_normal(path, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_parent(path, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_relative_to(base, other, result, buffer_size) bind(C)
import
character(kind=c_char), intent(in) :: base(*), other(*)
character(kind=c_char), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_proximate_to(base, other, result, buffer_size) bind(C)
import
character(kind=c_char), intent(in) :: base(*), other(*)
character(kind=c_char), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_which(name, path, find_all, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: name(*), path(*)
logical(C_BOOL), intent(in), value :: find_all
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_root(path, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_root_name(path, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_stem(path, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_suffix(path, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(C_BOOL) function fs_touch(path) bind(C)
import
character(kind=c_char), intent(in) :: path(*)
end function

integer(C_LONG_LONG) function fs_get_modtime(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

logical(C_BOOL) function fs_set_modtime(path) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
end function

integer(C_SIZE_T) function fs_with_suffix(path, new_suffix, result, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*), new_suffix
character(kind=C_CHAR), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_shortname(in, out, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: in(*)
character(kind=C_CHAR), intent(out) :: out(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_longname(in, out, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: in(*)
character(kind=C_CHAR), intent(out) :: out(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_lib_path(path, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(out) :: path(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_exe_path(path, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(out) :: path(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_backend(path, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(out) :: path(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

logical(C_BOOL) function fs_setenv(name, val) bind(C)
import
character(kind=C_CHAR), intent(in) :: name(*), val(*)
end function

integer(C_SIZE_T) function fs_filesystem_type(path, name, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(in) :: path(*)
character(kind=C_CHAR), intent(out) :: name(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_cpu_arch(arch, buffer_size) bind(C)
import
character(kind=C_CHAR), intent(out) :: arch(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_to_cygpath(path, result, buffer_size) bind(C)
import
character(kind=c_char), intent(in) :: path(*)
character(kind=c_char), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

integer(C_SIZE_T) function fs_to_winpath(path, result, buffer_size) bind(C)
import
character(kind=c_char), intent(in) :: path(*)
character(kind=c_char), intent(out) :: result(*)
integer(C_SIZE_T), intent(in), value :: buffer_size
end function

end interface

#ifndef NO_F03TYPE
include "path_type.inc"
#endif

contains

!> non-functional API

integer function max_path()
!! returns dynamic MAX_PATH for this computer
!! Maximum path length is dynamically determined for this computer.
!! A fixed length eases sending data to/from C/C++.
!!
!! Physical filesystem maximum filename and path lengths are OS and config dependent.
!! Notional limits:
!! MacOS: 1024 from sys/syslimits.h PATH_MAX
!! Linux: 4096 from https://www.ibm.com/docs/en/spectrum-protect/8.1.13?topic=parameters-file-specification-syntax
!! Windows: 32767 from https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation?tabs=cmd
max_path = int(fs_get_max_path())
end function


#ifndef NO_F03TYPE
include "path_methods.inc"
#endif


integer function max_component(path) result (r)
!! returns maximum component length for this filesystem
!! Maximum component length is dynamically determined for this computer.
character(*), intent(in) :: path
r = int(fs_max_component(trim(path) // C_NULL_CHAR))
end function


function as_posix(path) result(r)
!! force Posix file separator "/"
character(*), intent(in) :: path
include "ifc0a.inc"
N = fs_as_posix(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


function canonical(path, strict, expand_tilde) result (r)
include "ifc1a.inc"
N = fs_canonical(trim(path) // C_NULL_CHAR, s, e, cbuf, N)
include "ifc0b.inc"
end function


function realpath(path) result (r)
character(*), intent(in) :: path
include "ifc0a.inc"
N = fs_realpath(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


function resolve(path, strict, expand_tilde) result(r)
include "ifc1a.inc"
N = fs_resolve(trim(path) // C_NULL_CHAR, s, e, cbuf, N)
include "ifc0b.inc"
end function


subroutine set_permissions(path, readable, writable, executable, ok)
!! set owner readable bit for regular file
character(*), intent(in) :: path
logical, intent(in), optional :: readable, writable, executable
logical, intent(out), optional :: ok

logical(C_BOOL) :: s

integer(C_INT) :: r, w, e

r = 0
w = 0
e = 0

if(present(readable)) then
  r = -1
  if(readable) r = 1
end if
if(present(writable)) then
  w = -1
  if(writable) w = 1
end if
if(present(executable)) then
  e = -1
  if(executable) e = 1
end if

s = fs_set_permissions(trim(path) // C_NULL_CHAR, r, w, e)
if (.not. s) write(stderr, '(/,A,1x,L1,1x,L1,1x,L1)') "ERROR: set_permissions(" // path // ")", r, w, e

if(present(ok)) then
  ok = s
elseif (.not. s) then
  error stop
end if
end subroutine


character(9) function get_permissions(path) result (r)
!! get permissions as POSIX string
character(*), intent(in) :: path
character(kind=c_char, len=:), allocatable :: cbuf
integer(C_SIZE_T) :: N
allocate(character(10) :: cbuf)
N = fs_get_permissions(trim(path) // C_NULL_CHAR, cbuf, len(cbuf, kind=C_SIZE_T))
if(N > 9) error stop "Error:Ffilesystem:get_permissions: unexpected length /= 9"
r = cbuf(:N)
end function


subroutine copy_file(src, dest, overwrite, ok)
!! copy single file from src to dest
character(*), intent(in) :: src, dest
logical, intent(in), optional :: overwrite
logical, intent(out), optional :: ok

logical(c_bool) :: ow, s
ow = .false.
if(present(overwrite)) ow = overwrite
s = fs_copy_file(trim(src) // C_NULL_CHAR, trim(dest) // C_NULL_CHAR, ow)
if(.not. s) write(stderr, '(a)') "ERROR:Ffilesystem:copy_file: failed to copy file: " // trim(src) // " to " // trim(dest)

if (present(ok)) then
  ok = s
elseif(.not. s) then
  error stop
end if
end subroutine


subroutine mkdir(path, ok)
!! create a directory, with parents if needed
character(*), intent(in) :: path
logical, intent(out), optional :: ok

logical(C_BOOL) :: s

s = fs_mkdir(trim(path) // C_NULL_CHAR)
if(.not. s) write(stderr,'(a,i0)') "ERROR:Ffilesystem:mkdir: failed to create directory: " // trim(path)

if(present(ok)) then
  ok = s
elseif (.not. s) then
  error stop
end if
end subroutine


function read_symlink(path) result (r)
!! resolve symbolic link--error/empty if not symlink
character(*), intent(in) :: path

include "ifc0a.inc"
N = fs_read_symlink(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


subroutine create_symlink(tgt, link, ok)
character(*), intent(in) :: tgt, link
logical, intent(out), optional :: ok

logical(C_BOOL) :: s

s = fs_create_symlink(trim(tgt) // C_NULL_CHAR, trim(link) // C_NULL_CHAR)
if(.not. s) write(stderr,'(a,1x,i0)') "ERROR:Ffilesystem:create_symlink: " // trim(link)

if(present(ok)) then
  ok = s
elseif (.not. s) then
  error stop
end if
end subroutine


logical function exists(path) result(r)
!! a file or directory exists
character(*), intent(in) :: path
r = fs_exists(trim(path) // C_NULL_CHAR)
end function


logical function is_removable(path) result(r)
!! is path removable like a USB drive or CD/DVD/Bluray
character(*), intent(in) :: path
r = fs_is_removable(trim(path) // C_NULL_CHAR)
end function


function expanduser(path) result (r)
!! expand ~ to home directory
character(*), intent(in) :: path

include "ifc0a.inc"
N = fs_expanduser(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


function file_name(path) result (r)
!! returns file name without path
character(*), intent(in) :: path

include "ifc0a.inc"
N = fs_file_name(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


logical function is_safe_name(filename) result(r)
!! is filename a safe name for this filesystem
character(*), intent(in) :: filename

r = fs_is_safe_name(trim(filename) // C_NULL_CHAR)
end function


logical function is_empty(path)
!! is directory or file empty
character(*), intent(in) :: path

is_empty = fs_is_empty(trim(path) // C_NULL_CHAR)
end function


integer(int64) function file_size(path) result(r)
!! size of file (bytes)
character(*), intent(in) :: path

r = fs_file_size(trim(path) // C_NULL_CHAR)
end function


integer(int64) function hard_link_count(path) result(r)
!! size of file (bytes)
character(*), intent(in) :: path

r = fs_hard_link_count(trim(path) // C_NULL_CHAR)
end function


integer(int64) function space_available(path) result(r)
!! returns space available (bytes) on filesystem containing path
character(*), intent(in) :: path

r = fs_space_available(trim(path) // C_NULL_CHAR)
end function


integer(int64) function space_capacity(path) result(r)
!! returns space capacity (bytes) on filesystem containing path
character(*), intent(in) :: path

r = fs_space_capacity(trim(path) // C_NULL_CHAR)
end function


logical function is_absolute(path) result(r)
!! is path absolute
!! do NOT expanduser() to be consistent with Python etc. filesystem
character(*), intent(in) :: path

r = fs_is_absolute(trim(path) // C_NULL_CHAR)
end function


logical function is_appexec_alias(path) result(r)
!! is path a Windows App executable alias
character(*), intent(in) :: path

r = fs_is_appexec_alias(trim(path) // C_NULL_CHAR)
end function


logical function is_char_device(path) result(r)
!! is path a character device like /dev/null
character(*), intent(in) :: path

r = fs_is_char_device(trim(path) // C_NULL_CHAR)
end function


logical function is_fifo(path) result(r)
!! is path a FIFO (named pipe)
character(*), intent(in) :: path

r = fs_is_fifo(trim(path) // C_NULL_CHAR)
end function


logical function is_case_sensitive(path) result (r)
!! is filesystem case sensitive
character(*), intent(in) :: path

r = fs_is_case_sensitive(trim(path) // C_NULL_CHAR)
end function


logical function is_dir(path) result(r)
!! .true.: "path" is a directory OR symlink pointing to a directory
!! .false.: "path" is a broken symlink, does not exist, or is some other type of filesystem entity
character(*), intent(in) :: path

r = fs_is_dir(trim(path) // C_NULL_CHAR)
end function


logical function is_exe(path) result(r)
!! is "path" executable?
character(*), intent(in) :: path

r = fs_is_exe(trim(path) // C_NULL_CHAR)
end function


logical function is_executable_binary(path) result(r)
!! is "path" an executable binary?
character(*), intent(in) :: path

r = fs_is_executable_binary(trim(path) // C_NULL_CHAR)
end function


logical function is_readable(path) result(r)
!! is "path" readable?
character(*), intent(in) :: path

r = fs_is_readable(trim(path) // C_NULL_CHAR)
end function


logical function is_writable(path) result(r)
!! is "path" writable?
character(*), intent(in) :: path

r = fs_is_writable(trim(path) // C_NULL_CHAR)
end function


logical function is_file(path) result(r)
!! .true.: "path" is a file OR symlink pointing to a file
!! .false.: "path" is a directory, broken symlink, or does not exist
character(*), intent(in) :: path

r = fs_is_file(trim(path) // C_NULL_CHAR)
end function


logical function is_other(path) result(r)
character(*), intent(in) :: path
r = fs_is_other(trim(path) // C_NULL_CHAR)
end function


logical function is_reserved(path) result(r)
!! .true.: "path" is a reserved name on this filesystem
character(*), intent(in) :: path

r = fs_is_reserved(trim(path) // C_NULL_CHAR)
end function


logical function is_symlink(path) result(r)
!! .true.: "path" is a symbolic link
!! .false.: "path" is not a symbolic link, or does not exist,
!!           or platform/drive not capable of symlinks
character(*), intent(in) :: path

r = fs_is_symlink(trim(path) // C_NULL_CHAR)
end function


function join(path, other) result (r)
!! Join path with other path string using posix separators.
!! The paths are treated like strings.
!! Mo path resolution is used, so non-sensical paths are possible for non-sensical input.
character(*), intent(in) :: path, other

include "ifc0a.inc"
N = fs_join(trim(path) // C_NULL_CHAR, trim(other) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


logical function is_prefix(prefix, path) result (r)
!! is prefix a prefix of path
character(*), intent(in) :: prefix, path

r = fs_is_prefix(trim(prefix) // C_NULL_CHAR, trim(path) // C_NULL_CHAR)
end function


logical function is_subdir(subdir, dir) result (r)
!! is subdir a subdirectory of dir
character(*), intent(in) :: subdir, dir

r = fs_is_subdir(trim(subdir) // C_NULL_CHAR, trim(dir) // C_NULL_CHAR)
end function


function parent(path) result(r)
!! returns parent directory of path
character(*), intent(in) :: path

include "ifc0a.inc"
N = fs_parent(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


function normal(path) result (r)
!! lexically normalize path
character(*), intent(in) :: path

include "ifc0a.inc"
N = fs_normal(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


function relative_to(base, other) result (r)
!! returns other relative to base
!! if other is not a subpath of base, returns "" empty string
!!
!! reference: C++ filesystem relative
!! https://en.cppreference.com/w/cpp/filesystem/relative
character(*), intent(in) :: base, other

include "ifc0a.inc"
N = fs_relative_to(trim(base) // C_NULL_CHAR, trim(other) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


function proximate_to(base, other) result (r)
!! returns other proximate to base
!! if other is not a subpath of base, returns "" empty string
!!
!! reference: C++ filesystem proximate
!! https://en.cppreference.com/w/cpp/filesystem/proximate
character(*), intent(in) :: base, other

include "ifc0a.inc"
N = fs_proximate_to(trim(base) // C_NULL_CHAR, trim(other) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


function root(path) result (r)
!! returns root of path
character(*), intent(in) :: path

include "ifc0a.inc"
N = fs_root(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


function root_name(path) result(r)
!! returns root name of path
character(*), intent(in) :: path

include "ifc0a.inc"
N = fs_root_name(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


logical function same_file(path1, path2)
character(*), intent(in) :: path1, path2

same_file = fs_equivalent(trim(path1) // C_NULL_CHAR, trim(path2) // C_NULL_CHAR)
end function


function stem(path) result (r)
character(*), intent(in) :: path

include "ifc0a.inc"
N = fs_stem(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


function suffix(path) result (r)
!! extracts path suffix, including the final "." dot
character(*), intent(in) :: path

include "ifc0a.inc"
N = fs_suffix(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


subroutine touch(path, ok)
character(*), intent(in) :: path
logical, intent(out), optional :: ok

logical(C_BOOL) :: s

s = fs_touch(trim(path) // C_NULL_CHAR)
if(.not. s) write(stderr, '(a)') "ERROR:Ffilesystem: touch(" // trim(path) // ") failed."

if(present(ok)) then
  ok = s
elseif(.not. s) then
  error stop
end if
end subroutine


integer(C_LONG_LONG) function get_modtime(path) result(r)
!! get file modification time as Unix epoch time
character(*), intent(in) :: path

r = fs_get_modtime(trim(path) // C_NULL_CHAR)
end function


logical function set_modtime(path) result(r)
!! get file modification time as Unix epoch time
character(*), intent(in) :: path

r = fs_set_modtime(trim(path) // C_NULL_CHAR)
end function


function with_suffix(path, new) result(r)
!! replace file suffix with new suffix
character(*), intent(in) :: path,new

include "ifc0a.inc"
N = fs_with_suffix(trim(path) // C_NULL_CHAR, trim(new) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


function shortname(path) result(r)
!! convert long path to short
character(*), intent(in) :: path

include "ifc0a.inc"
N = fs_shortname(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


function longname(path) result (r)
!! convert short path to long
character(*), intent(in) :: path

include "ifc0a.inc"
N = fs_longname(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


function to_cygpath(path) result (r)
!! convert native Windows to Cygwin path
character(*), intent(in) :: path

include "ifc0a.inc"
N = fs_to_cygpath(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


function to_winpath(path) result (r)
!! convert Cygwin path to native Windows
character(*), intent(in) :: path

include "ifc0a.inc"
N = fs_to_winpath(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


subroutine setenv(name, val, ok)
!! set environment variable "name" to value "val"
character(*), intent(in) :: name, val
logical, intent(out), optional :: ok

logical(C_BOOL) :: s

s = fs_setenv(trim(name) // C_NULL_CHAR, trim(val) // C_NULL_CHAR)
if(.not. s) write(stderr,'(a,1x,i0)') "ERROR:Ffilesystem: setenv(" // trim(name) // ") failed."

if(present(ok)) then
  ok = s
elseif (.not. s) then
  error stop
end if
end subroutine


function getarg(index) result(r)
!! get command argument
integer, intent(in) :: index
character(:), allocatable :: r
integer :: i, L

call get_command_argument(index, length=L, status=i)

if (i/=0) then
  r = ""
  return
end if

allocate(character(L) :: r)
call get_command_argument(index, value=r)

end function


function getenv(name) result(r)
!! get environment variable
character(*), intent(in) :: name
character(:), allocatable :: r
integer :: i, L

call get_environment_variable(name, length=L, status=i)

if (i/=0) then
  r = ""
  return
end if

allocate(character(L) :: r)
call get_environment_variable(name, value=r)

end function


function backend() result(r)
!! Ffilesystem backend
include "ifc0a.inc"
N = fs_backend(cbuf, N)
include "ifc0b.inc"
end function


function exe_path() result (r)
!! get full path of main executable

include "ifc0a.inc"
N = fs_exe_path(cbuf, N)
include "ifc0b.inc"
end function
!> one-liner methods calling actual procedures


subroutine assert_is_file(path)
!! throw error if file does not exist
character(*), intent(in) :: path
if (.not. is_file(path)) then
  write(stderr, '(a)') 'File does not exist: ' // path
  error stop
end if
end subroutine


subroutine assert_is_dir(path)
!! throw error if directory does not exist
character(*), intent(in) :: path
if (.not. is_dir(path)) then
  write(stderr, '(a)') 'Directory does not exist: ' // path
  error stop
end if
end subroutine


function which(name, path, find_all) result(r)
!! find executable in PATH
character(*), intent(in) :: name
character(*), intent(in), optional :: path
logical, intent(in), optional :: find_all
logical(C_BOOL) :: a

include "ifc0a.inc"

a = .false.
if(present(find_all)) a = find_all

if(present(path)) then
  N = fs_which(trim(name) // C_NULL_CHAR, trim(path) // C_NULL_CHAR, a, cbuf, N)
else
  N = fs_which(trim(name) // C_NULL_CHAR, C_NULL_CHAR, a, cbuf, N)
end if

include "ifc0b.inc"
end function


subroutine remove(path, ok)
!! delete the file, symbolic link, or empty directory
character(*), intent(in) :: path
logical, intent(out), optional :: ok

logical(c_bool) :: s

s = fs_remove(trim(path) // C_NULL_CHAR)
if (.not. s) write(stderr, '(a)') "ERROR:Ffs:remove: " // trim(path) // " may not have been deleted."

if(present(ok)) then
  ok = s
elseif (.not. s) then
  error stop
end if
end subroutine


subroutine fs_rename(old, new, ok)
!! rename file or directory
character(*), intent(in) :: old, new
logical, intent(out), optional :: ok

logical(c_bool) :: s

s = fs_rename_c(trim(old) // C_NULL_CHAR, trim(new) // C_NULL_CHAR)
if(.not. s) write(stderr, '(a)') "ERROR:Ffs:rename: " // trim(old) // " to " // trim(new) // " may not have been renamed."

if(present(ok)) then
  ok = s
else if (.not. s) then
  error stop
end if

end subroutine


function compiler_c() result(r)
!! get C/C++ compiler name and version
include "ifc0a.inc"
N = fs_compiler(cbuf, N)
include "ifc0b.inc"
end function


function get_shell() result(r)
!! get shell name
include "ifc0a.inc"
N = fs_get_shell(cbuf, N)
include "ifc0b.inc"
end function


function get_terminal() result(r)
!! get terminal name
include "ifc0a.inc"
N = fs_get_terminal(cbuf, N)
include "ifc0b.inc"
end function


character function pathsep()
!! path separater (not file separator)
!! we do this discretely in Fortran as there is a long-standing bug
!! in nvfortran with passing single characters from C to Fortran caller
if (is_windows()) then
  pathsep = ";"
else
  pathsep = ":"
end if
end function


function compiler() result(r)
character(:), allocatable :: r
r = compiler_version()
end function


function lib_path() result (r)
!! get full path of shared library. Empty if not shared library.
include "ifc0a.inc"
N = fs_lib_path(cbuf, N)
include "ifc0b.inc"
end function


function get_cwd() result (r)
!! get current working directory

include "ifc0a.inc"
N = fs_get_cwd(cbuf, N)
include "ifc0b.inc"
end function


logical function set_cwd(path)
!! set current working directory (chdir)
character(*), intent(in) :: path

set_cwd = fs_set_cwd(trim(path) // C_NULL_CHAR)
end function


function user_config_dir() result (r)
!! get user configuration directory
include "ifc0a.inc"
N = fs_user_config_dir(cbuf, N)
include "ifc0b.inc"
end function


function get_homedir() result (r)
!! returns home directory, or empty string if not found
!!
!! https://en.wikipedia.org/wiki/Home_directory#Default_home_directory_per_operating_system

include "ifc0a.inc"
N = fs_get_homedir(cbuf, N)
include "ifc0b.inc"
end function


function get_profile_dir() result (r)
!! returns profile directory
!! normally prefer to use get_homedir(), which falls back to get_profile_dir()

include "ifc0a.inc"
N = fs_get_profile_dir(cbuf, N)
include "ifc0b.inc"
end function


function hostname() result (r)
!! get hostname

include "ifc0a.inc"
N = fs_hostname(cbuf, N)
include "ifc0b.inc"
end function


function get_username() result (r)
!! get username

include "ifc0a.inc"
N = fs_get_username(cbuf, N)
include "ifc0b.inc"
end function


function get_owner_name(path) result (r)
!! get owner name of file or directory
character(*), intent(in) :: path

include "ifc0a.inc"
N = fs_get_owner_name(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


function get_owner_group(path) result (r)
!! get owner group of file or directory
character(*), intent(in) :: path

include "ifc0a.inc"
N = fs_get_owner_group(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


function get_tempdir() result (r)
!! get system temporary directory

include "ifc0a.inc"
N = fs_get_tempdir(cbuf, N)
include "ifc0b.inc"
end function


function absolute(path, base, expand_tilde) result(r)
!! if path is absolute, return expanded path
!! if path is relative, base / path
!!
!! idempotent iff base is absolute
character(*), intent(in) :: path, base
logical, intent(in), optional :: expand_tilde

logical(C_BOOL) :: e

include "ifc0a.inc"

e = .true.
if(present(expand_tilde)) e = expand_tilde

N = fs_absolute(trim(path) // C_NULL_CHAR, trim(base) // C_NULL_CHAR, e, cbuf, N)
include "ifc0b.inc"
end function


function filesystem_type(path) result (r)
!! get filesystem type of path, which must exist
!! returns empty string if path does not exist or type cannot be determined
character(*), intent(in) :: path

include "ifc0a.inc"
N = fs_filesystem_type(trim(path) // C_NULL_CHAR, cbuf, N)
include "ifc0b.inc"
end function


function devnull()
!! get path to /dev/null or equivalent
character(:), allocatable :: devnull

if(is_windows()) then
  devnull = "nul"
else
  devnull = "/dev/null"
end if
end function


function cpu_arch() result(r)
!! get CPU architecture

include "ifc0a.inc"
N = fs_cpu_arch(cbuf, N)
include "ifc0b.inc"

end function


elemental logical function is_alpha(c) result(r)
!! is character alphabetic in the ASCII alphabet
character, intent(in) :: c

r = (c >= 'A' .and. c <= 'Z') .or. (c >= 'a' .and. c <= 'z')
end function

end module filesystem
