# AppleClang needs a hint to find Flang ISO_Fortran_binding.h since they are distinct compiler vendors
if(CMAKE_C_COMPILER_ID STREQUAL "AppleClang" AND CMAKE_Fortran_COMPILER_ID STREQUAL "LLVMFlang")
  execute_process(
    COMMAND ${CMAKE_Fortran_COMPILER} -print-resource-dir
    OUTPUT_VARIABLE flang_resource_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE _ret
  )
  if(_ret EQUAL 0)
    message(DEBUG "Flang resource dir: ${flang_resource_dir}")
    set(_flang_ifb_hint HINTS "${flang_resource_dir}/include" PATH_SUFFIXES flang NO_DEFAULT_PATH)
  else()
    set(_flang_ifb_hint)
  endif()
  find_path(ffilesystem_ISO_Fortran_binding_path NAMES ISO_Fortran_binding.h ${_flang_ifb_hint})
  if(ffilesystem_ISO_Fortran_binding_path)
    set(CMAKE_REQUIRED_INCLUDES ${ffilesystem_ISO_Fortran_binding_path})
  endif()
endif()

check_include_file("ISO_Fortran_binding.h" ffilesystem_ifb_ok)
if(ffilesystem_ifb_ok)
  if(NOT DEFINED ffilesystem_HAVE_ISO_FORTRAN_BINDING)

    message(CHECK_START "checking that ISO_Fortran_binding.h is usable for <string> interoperability")

    try_run(ffilesystem_ifb_run ffilesystem_ifb_build
    SOURCES ${CMAKE_CURRENT_LIST_DIR}/ifb.c ${CMAKE_CURRENT_LIST_DIR}/ifb.f90
    CMAKE_FLAGS -DINCLUDE_DIRECTORIES=${CMAKE_REQUIRED_INCLUDES}
    COMPILE_OUTPUT_VARIABLE _ifb_build
    RUN_OUTPUT_STDOUT_VARIABLE _ifb_code
    RUN_OUTPUT_STDERR_VARIABLE _ifb_error
    )

    string(STRIP "${_ifb_code}" _ifb_code)
    string(STRIP "${_ifb_error}" _ifb_error)

    if(NOT ffilesystem_ifb_build)
      message(CHECK_FAIL "failed to compile ISO_Fortran_binding.h ${ffilesystem_ifb_build}
      ${_ifb_build}")
    elseif(NOT ffilesystem_ifb_run EQUAL 0)
      message(CHECK_FAIL "failed to test ISO_Fortran_binding.h ${ffilesystem_ifb_run}
      ${_ifb_code}
        ${_ifb_error}")
    else()
      set(ffilesystem_HAVE_ISO_FORTRAN_BINDING true CACHE STRING "ISO_Fortran_binding.h test code")
      message(CHECK_PASS "OK: ${_ifb_code}")
    endif()

  endif()
endif()
