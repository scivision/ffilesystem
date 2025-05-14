program test_canon

use, intrinsic:: iso_fortran_env, only : stderr=>error_unit

use filesystem

implicit none

valgrind : block

character(:), allocatable :: p1, p2

integer :: L1, L2

! -- current directory  -- old MacOS doesn't handle "." or ".." alone
p1 = realpath(".")
L1 = len_trim(p1)
if(L1 == 0) error stop "FAILED: realpath '.'"

p2 = get_cwd()
L2 = len_trim(p2)
if(L1 /= L2) then
  write(stderr, '(a,1x,i0,1x,i0)')  "FAILED: realpath '.' failed: " // p1 // " /= " // p2, L1, L2
  error stop
endif

print *, "OK: current dir = ", p1


! -- relative dir
p1 = realpath("..")

L2 = len_trim(p1)
if (L2 >= L1) then
  write(stderr,'(a,1x,i0,1x,i0)') 'ERROR:realpath:relative: up dir not realpath: ~/.. => ' // p1, L1, L2
  error stop
end if
print *, 'OK: realpath(..) = ', p1

end block valgrind


end program
