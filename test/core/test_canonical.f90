program test_canon

use, intrinsic:: iso_fortran_env, only : stderr=>error_unit

use filesystem

implicit none

valgrind : block

character(:), allocatable :: p1, p2

integer :: L1, L2

! -- current directory  -- old MacOS doesn't handle "." or ".." alone
p1 = canonical(".")
L1 = len_trim(p1)
if(L1 == 0) error stop "ERROR: canonical '.' failed"

p2 = get_cwd()
if(p1 /= p2) error stop "ERROR: canonical '.' failed: " // p1 // " /= " // p2

print *, "OK: current dir = ", p1

! -- home directory
p2 = get_homedir()
p1 = canonical("~")
L1 = len_trim(p1)
if (p1(1:1) == "~") then
  write(stderr,'(a)') "canonical(~) did not expanduser: " // p1
  error stop
end if
if(len_trim(p1) /= len_trim(p2)) then
  write(stderr,*) "ERROR: canonical('~') " // p1 // " /= get_homedir: " // p2
  error stop
end if

print *, "OK: home dir = ", p1

!> empty
p2 = canonical("")
if(p2 /= "") error stop "canonical('') not empty: " // p2

! -- relative dir
p1 = canonical("~/..", .false., .true.)

L2 = len_trim(p1)
if (L2 >= L1) then
  write(stderr,*) 'ERROR:canonical:relative: up dir not canonicalized: ~/.. => ' // p1
  error stop
end if
print *, 'OK: canon_dir = ', p1

if(is_cygwin()) stop "OK: Cygwin does not support canonicalize relative non-existing path"

! -- relative, non-existing file

!> strict, not exist
if(backend() == "<filesystem>") then
p1 = canonical("not-exist/dir/..", strict=.true.)
!! not a trailing slash input to avoid ambiguity in various backends
if (len_trim(p1) /= 0) then
  write(stderr,*) 'ERROR: strict not-exist should return empty string: ' // p1
  error stop
end if
endif

print *, 'OK: canon_file = ', p1

end block valgrind


end program
