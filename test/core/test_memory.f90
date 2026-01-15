program test_owner

use, intrinsic :: iso_fortran_env

use filesystem

implicit none

integer(int64) :: f, t

f = free_memory()
t = total_sys_memory()

if (f <= 0) error stop 'free_memory() returned non-positive value'
if (t <= 0) error stop 'total_sys_memory() returned non-positive value'
if (f > t) error stop 'free_memory() returned value larger than total_sys_memory'

print '(a,i0)', 'Free memory: ', f
print '(a,i0)', 'Total system memory: ', t

print '(a)', 'memory test passed.'

end program
