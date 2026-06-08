if(ffilesystem_iso_fortran_binding)
  if(CMAKE_C_COMPILER_ID STREQUAL "AppleClang" AND CMAKE_Fortran_COMPILER_ID STREQUAL "LLVMFlang")
    find_program(brew_exe NAMES brew)
    if(brew_exe)
      execute_process(COMMAND ${brew_exe} --prefix flang
      OUTPUT_VARIABLE flang_prefix
      OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    endif()
    find_path(ffilesystem_ISO_Fortran_binding_path NAMES ISO_Fortran_binding.h
    HINTS ${flang_prefix}/include
    PATH_SUFFIXES flang)
    if(ffilesystem_ISO_Fortran_binding_path)
      set(CMAKE_REQUIRED_INCLUDES ${ffilesystem_ISO_Fortran_binding_path})
    endif()
  endif()

  check_include_file("ISO_Fortran_binding.h" ffilesystem_ifb_ok)
  if(ffilesystem_ifb_ok)
    check_source_compiles(Fortran
"program test
use, intrinsic :: iso_c_binding

interface
integer(C_INT) function t_cfi(a, r) bind(C)
import C_INT
character(*), intent(in)  :: a
character(*), intent(out) :: r
end function t_cfi
end interface

end program"
    ffilesystem_HAVE_ISO_FORTRAN_BINDING)
  endif()
endif()
