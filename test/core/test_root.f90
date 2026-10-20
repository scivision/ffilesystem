program test_root

use, intrinsic :: iso_fortran_env, only: stderr => error_unit
use filesystem

implicit none

character(*), parameter :: s1="C:\a\b", s2="C:/a/b"
character(*), parameter :: r1="C:\",    r2 = "C:/"
character(*), parameter :: rn="C:"

if(root("") /= "") error stop "root empty"

if(is_windows()) then
  if(root(s1) /= r1) error stop "root should be " // r1 // ", got " // root(s1)
  if(root(s2) /= r2) error stop "root should be " // r2 // ", got " // root(s2)

  if(root_name(s1) /= rn) error stop "root name should be " // rn // ", got " // root_name(s1)
  if(root_name(s2) /= rn) error stop "root name should be " // rn // ", got " // root_name(s2)
  if(.not. has_root_name(s1)) error stop "has_root_name should be true for " // s1

  if(root_directory(s1) /= "\") error stop "root directory should be \, got " // root_directory(s1)
  if(root_directory(s2) /= "/") error stop "root directory should be /, got " // root_directory(s2)
  if(.not. has_root_directory(s2)) error stop "has_root_directory should be true for " // s2

else

  if(root("/a/b") /= "/") error stop "relative root should be empty"
  if(root_name("/a/b") /= "") error stop "root name should be empty"
  if(root_directory("/a/b") /= "/") error stop "root directory should be /"

endif

print '(A)', "OK: root: Fortran"

end program
