include(CheckIncludeFile)

if(LINUX)
  check_include_file("linux/magic.h" ffilesystem_HAVE_LINUX_MAGIC)
endif()

if(HAVE_CXX_FILESYSTEM)
  set(_regex "Backend: <filesystem>")
else()
  set(_regex "Backend: C")
endif()


if(TARGET filesystem_cli)

add_test(NAME Fortran_CLI
COMMAND ${CMAKE_COMMAND} -Dexe:FILEPATH=$<TARGET_FILE:filesystem_cli> -P ${CMAKE_CURRENT_SOURCE_DIR}/stdin_nul.cmake
)
set_tests_properties(Fortran_CLI PROPERTIES
LABELS "Fortran"
PASS_REGULAR_EXPRESSION ${_regex}
)

endif()


if(TARGET fs_cli)

add_test(NAME Cpp_CLI
COMMAND ${CMAKE_COMMAND} -Dexe:FILEPATH=$<TARGET_FILE:fs_cli> -P ${CMAKE_CURRENT_SOURCE_DIR}/stdin_nul.cmake
)

set_tests_properties(Cpp_CLI PROPERTIES
LABELS "Cpp"
PASS_REGULAR_EXPRESSION ${_regex}
)

add_test(NAME CppCLInoLeak
COMMAND ${CMAKE_COMMAND} -Dexe:FILEPATH=$<TARGET_FILE:fs_cli> -P ${CMAKE_CURRENT_SOURCE_DIR}/stdin.cmake
)

set_tests_properties(CppCLInoLeak PROPERTIES
DISABLED $<AND:$<BOOL:${LINUX}>,$<NOT:$<BOOL:${ffilesystem_HAVE_LINUX_MAGIC}>>>
LABELS "Cpp"
)

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

set(test_dirs core)

# Do not use find_package(GTest), in general it finds some other library's GTest that may have an incompatible ABI.
include(FetchContent)

# set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(boost_ut
URL ${boost_ut_url}
)
FetchContent_MakeAvailable(boost_ut)


foreach(t IN LISTS test_dirs)

add_subdirectory(${t})

# --- Windows shared DLLs
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.22 AND BUILD_SHARED_LIBS AND (CYGWIN OR WIN32))

  get_property(n DIRECTORY ${t} PROPERTY TESTS)

  set_property(TEST ${n} DIRECTORY ${t}
  PROPERTY ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:ffilesystem>"
  )
endif()

endforeach()

# --- coverage

if(${PROJECT_NAME}_coverage)
  setup_target_for_coverage_gcovr_html(
  NAME coverage
  EXECUTABLE ${CMAKE_CTEST_COMMAND}
  )
endif()
