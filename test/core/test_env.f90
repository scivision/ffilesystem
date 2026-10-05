module test_env_callback

use, intrinsic :: iso_c_binding, only : c_char, c_null_char

implicit none
integer :: diagnostic_count = 0

contains

subroutine report_error(message) bind(C)
character(c_char), intent(in) :: message(*)
if (message(1) == c_null_char) error stop "empty disabled-system diagnostic"
diagnostic_count = diagnostic_count + 1
end subroutine

end module

program test_env

use, intrinsic :: iso_c_binding, only : c_funloc
use, intrinsic :: iso_fortran_env, only : stderr=>error_unit
use filesystem
use test_env_callback

implicit none
character(8) :: mode
logical :: system_enabled

call get_command_argument(1, mode)
system_enabled = mode /= "disabled"

call test_exists()
print '(a)', "OK fs: exists"

call test_homedir()
print '(a)', "OK fs: homedir"

if (len_trim(get_tempdir()) == 0) error stop "get_tempdir failed"
print '(a)', "OK: get_tempdir: " // get_tempdir()

call test_username()
print '(a)', "OK fs: username"


contains


subroutine test_exists()

if(exists("")) error stop "empty does not exist"

if(.not. exists(get_cwd())) error stop "exists(get_cwd) failed"

end subroutine


subroutine test_homedir()

character(:), allocatable :: h, p, k, buf

if(is_windows()) then
  k = "USERPROFILE"
else
  k = "HOME"
end if

buf = getenv(k)
if (len_trim(buf) == 0) then
  print '(a)', "env var " // k // " not set"
else
  print '(a)', "getenv: " // k // " = " // trim(buf)
end if

h = get_homedir()
if (len_trim(h) == 0) error stop "get_homedir failed"

diagnostic_count = 0
if (.not. system_enabled) call set_error_callback(c_funloc(report_error))
p = get_profile_dir()
if (.not. system_enabled) call reset_error_callback()
if (system_enabled) then
  if (len_trim(p) == 0) error stop "get_profile_dir failed"
else
  if (len_trim(p) /= 0) error stop "disabled get_profile_dir should be empty"
  if (diagnostic_count /= 1) error stop "missing disabled get_profile_dir diagnostic"
end if

print '(a)', "OK: get_homedir: " // h

end subroutine


subroutine test_username()

character(:), allocatable :: u

diagnostic_count = 0
if (.not. system_enabled) call set_error_callback(c_funloc(report_error))
u = get_username()
if (.not. system_enabled) call reset_error_callback()
if (system_enabled) then
  if (len_trim(u) == 0) error stop "get_username failed"
else
  if (len_trim(u) /= 0) error stop "disabled get_username should be empty"
  if (diagnostic_count /= 1) error stop "missing disabled get_username diagnostic"
end if

print '(a)', "OK: get_username: " // u

end subroutine


end program
