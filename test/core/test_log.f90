module test_log_callback

use, intrinsic :: iso_c_binding, only : c_char, c_null_char

implicit none

character(2048) :: my_logger

contains

subroutine report_error(message) bind(C)
	character(c_char), intent(in) :: message(*)

	integer :: i

	my_logger = ""
	do i = 1, len(my_logger)
		if (message(i) == c_null_char) exit
		my_logger(i:i) = achar(iachar(message(i)))
	end do
end subroutine

end module

program test_log

use, intrinsic :: iso_c_binding, only : c_funloc, c_null_funptr
use filesystem
use test_log_callback

implicit none

character(1) :: argument

call get_command_argument(1, argument)
if (argument /= "0" .and. argument /= "1") error stop "expected logging mode 0 or 1"

if (argument == "0") then
	!> Test suppressed diagnostic output.
	call set_error_callback(c_null_funptr)
	if (file_size("not-a-file") /= fs_unknown_size) error stop "file_size() failed"

	!> Test an application callback storing the diagnostic in a Fortran variable.
	my_logger = ""
	call set_error_callback(c_funloc(report_error))
	if (file_size("not-a-file") /= fs_unknown_size) error stop "file_size() failed"
	if (index(my_logger, "ERROR: Ffilesystem:") == 0) error stop "missing diagnostic"
	if (index(my_logger, "not-a-file") == 0) error stop "missing path"
else
	!> Test the default stderr diagnostic callback.
	call reset_error_callback()
	if (file_size("not-a-file") /= fs_unknown_size) error stop "file_size() failed"
end if

call reset_error_callback()

end program
