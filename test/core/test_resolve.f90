program test_res

use, intrinsic:: iso_fortran_env, only : stderr=>error_unit

use filesystem

implicit none

valgrind : block

character(:), allocatable :: p1, cwd, n

integer :: L1, L3

! -- current directory  -- old MacOS doesn't handle "." or ".." alone
p1 = resolve(".")
L1 = len_trim(p1)
if (L1 <= 1) then
  write(stderr,*) "ERROR: resolve '.' " // p1
  error stop
end if

cwd = get_cwd()
n = file_name(cwd)

if (file_name(p1) /= n) then
  write(stderr,*) "ERROR: resolve('.') " // p1 // " /= " // cwd
  error stop
end if

print *, "OK: current dir = ", p1

!> empty
if(file_name(resolve("")) /= n) then
  write(stderr,*) "resolve('') " // resolve("") // " /= " // cwd
  error stop
end if

! -- relative dir
p1 = resolve("..")
L3 = len_trim(p1)
if (L3 == 0) then
  write(stderr,*) 'ERROR:resolve:relative: up dir not resolved: .. => ' // p1, L3
  error stop
end if
print *, 'OK: canon_dir = ', p1

if(is_cygwin()) stop "OK: Cygwin does not support canonicalize relative non-existing path"


! -- relative, non-existing file

!> strict, not exist
if(backend() == "<filesystem>") then
p1 = resolve("not-exist/dir/..", strict=.true.)
!! not a trailing slash input to avoid ambiguity in various backends
if (len_trim(p1) /= 0) then
  write(stderr,*) 'failed: resolve(strict not-exist should return empty string: ' // p1
  error stop
end if
endif

end block valgrind

print '(a)', 'OK: resolve'

end program
