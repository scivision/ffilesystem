program test_canon

use, intrinsic:: iso_fortran_env, only : stderr=>error_unit

use filesystem

implicit none

valgrind : block

character(:), allocatable :: p1

integer :: L1, L2

p1 = canonical("")
if(p1 /= "") error stop "canonical('') not empty: " // p1

p1 = canonical("~")
L1 = len_trim(p1)
p1 = canonical("~/..", .false., .true.)

L2 = len_trim(p1)
if (L2 >= L1) then
  write(stderr,*) 'ERROR:canonical:relative: up dir not canonicalized: ~/.. => ' // p1
  error stop
end if
print *, 'OK: canon_dir = ', p1

! -- relative, non-existing file

!> strict, not exist
if(backend() == "<filesystem>" .and. .not. is_cygwin()) then
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
