program test_pure

use filesystem

implicit none

call test_stem()
print '(a)', "OK: stem"

call test_suffix()
print '(a)', "OK: suffix"

contains

subroutine test_stem()
character(:), allocatable :: s

s = stem("")
if(s /= "") error stop "stem empty: " // s

s = stem("stem.a.b")
if (s /= "stem.a") error stop "%stem failed: " // s
s = stem(s)
if (s /= "stem") error stop "stem nest failed: " // s

end subroutine test_stem


subroutine test_suffix()

character(:), allocatable :: s1

if(suffix("") /= "") error stop "suffix empty"

s1 = suffix("suffix_name.a.b")

if (s1 /= ".b") error stop "suffix failed: " // s1

s1 = suffix(s1)
if (s1 /= "") error stop "suffix recursive failed: " // s1

end subroutine test_suffix

end program
