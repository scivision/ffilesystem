include(CheckIncludeFile)

if(LINUX)
  check_include_file("linux/magic.h" ffilesystem_HAVE_LINUX_MAGIC)
endif()

# -- CLI tests

if(NOT CMAKE_CROSSCOMPILING)
  include(${CMAKE_CURRENT_LIST_DIR}/cli_tests.cmake)
endif()


set(ffilesystem_fortran_test_flags)
if(CMAKE_Fortran_COMPILER_ID STREQUAL "GNU" AND CMAKE_Fortran_COMPILER_VERSION VERSION_LESS 13)
  set(ffilesystem_fortran_test_flags -Wno-maybe-uninitialized)
endif()
message(VERBOSE "${CMAKE_Fortran_COMPILER_ID} ${CMAKE_Fortran_COMPILER_VERSION}: Fortran test flags: ${ffilesystem_fortran_test_flags}")

set(test_dirs core symlink)
if(ffilesystem_extra)
  list(APPEND test_dirs extra libpath)
endif()


include(${CMAKE_CURRENT_LIST_DIR}/GoogleTest.cmake)


foreach(t IN LISTS test_dirs)

add_subdirectory(${t})

# --- Windows shared DLLs
if(BUILD_SHARED_LIBS AND (CYGWIN OR WIN32))

  get_property(n DIRECTORY ${t} PROPERTY TESTS)

  set_property(TEST ${n} DIRECTORY ${t}
  PROPERTY ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:ffilesystem>"
  )
endif()

endforeach()
