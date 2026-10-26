program test_root

use, intrinsic :: iso_fortran_env, only: stderr => error_unit
use filesystem

implicit none

character(*), parameter :: rd1=achar(92), rd2="/"
character(*), parameter :: s1="C:"//rd1//"a"//rd1//"b", s2="C:/a/b"
character(*), parameter :: r1="C:"//rd1,    r2 = "C:/"
character(*), parameter :: rn="C:"

if(root("") /= "") error stop "root empty"

if(is_windows()) then
  print '(2a)', "s1: ", s1, " r1: ", r1
  if(root(s1) /= r1) error stop "root("// s1 // ") " // root(s1) // " != " // r1
  if(root(s2) /= r2) error stop "root("// s2 // ") " // root(s2) // " != " // r2

  if(root_name(s1) /= rn) error stop "root name " // root_name(s1) // " != " // rn
  if(root_name(s2) /= rn) error stop "root name " // root_name(s2) // " != " // rn
  if(.not. has_root_name(s1)) error stop "has_root_name " // s1 // " should be true"

  if(root_directory(s1) /= rd1) error stop "root directory " // root_directory(s1) // " != " // rd1
  if(root_directory(s2) /= rd2) error stop "root directory " // root_directory(s2) // " != " // rd2
  if(.not. has_root_directory(s2)) error stop "has_root_directory " // s2 // " should be true"

else

  if(root("/a/b") /= "/") error stop "relative root should be empty"
  if(root_name("/a/b") /= "") error stop "root name should be empty"
  if(root_directory("/a/b") /= "/") error stop "root directory should be /"

endif

print '(A)', "OK: root: Fortran"

end program
