program test_res

use, intrinsic:: iso_fortran_env, only : stderr=>error_unit

use filesystem

implicit none

valgrind : block

character(:), allocatable :: p1, p2, cwd

integer :: L1, L2, L3

! -- current directory  -- old MacOS doesn't handle "." or ".." alone
p1 = resolve(".")
L1 = len_trim(p1)
if (L1 < 1) then
  write(stderr,*) "ERROR: resolve '.' " // p1
  error stop
end if

cwd = get_cwd()

if (.not. same_file(p1, cwd)) then
  write(stderr,*) "ERROR: resolve('.') " // p1 // " /= " // cwd
  error stop
end if

print *, "OK: current dir = ", p1

! -- home directory
p2 = get_homedir()
p1 = resolve("~")

if (p1(1:1) == "~") then
  write(stderr,'(a)') "%resolve ~ did not expanduser: " // p1
  error stop
end if
if(len_trim(p1) /= len_trim(p2)) then
  write(stderr,*) "ERROR: resolve('~') " // p1 // " /= get_homedir: " // p2
  error stop
end if
print *, "OK: home dir = ", p1

p2 = parent(p1)
L2 = len_trim(p2)
if (L2 >= L1) error stop "parent home " // p2
print *, "OK: parent(resolve(~)) = ", p2

!> empty
if(.not. same_file(resolve(""), cwd)) then
  write(stderr,*) "resolve('') " // resolve("") // " /= " // cwd
  error stop
end if

! -- relative dir
p1 = resolve("~/..")
L3 = len_trim(p1)
if (L2 /= L3) then
  write(stderr,*) 'ERROR:resolve:relative: up dir not resolved: ~/.. => ' // p1, L3, L2
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
