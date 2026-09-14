character(:), allocatable :: r

character(kind=c_char, len=:), allocatable :: cbuf
integer(C_SIZE_T) :: N

allocate(character(max_path() + 1) :: cbuf)

N = len(cbuf)
