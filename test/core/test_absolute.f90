program main

use filesystem
use, intrinsic :: iso_fortran_env, only: stderr=> error_unit

implicit none

valgrind : block

character(:), allocatable :: in, base, ref, out

in = "rel"

if (is_windows()) then
  base = "j:/foo"
  ref = "j:/foo/rel"
else
  base = "/foo"
  ref = "/foo/rel"
end if

out = absolute(in, base)
if(len_trim(out) == 0) error stop "absolute() has empty output"
if (out /= ref) then
  write(stderr, '(a)') "Mismatch: absolute(" // in //", " // base // ") " // out // " /= " // ref
  error stop
endif

end block valgrind

end program
