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

# Do not use find_package(GTest), in general it finds some other library's GTest that may have an incompatible ABI.
include(FetchContent)

set(FETCHCONTENT_QUIET OFF)
set(INSTALL_GTEST OFF)

FetchContent_Declare(GTest
URL "${ffilesystem_googletest_url}"
FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(GTest)

if(CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM")
  foreach(t IN ITEMS gtest gtest_main gmock gmock_main)
    if(WIN32)
      # necessary since GoogleTest injects /WX blindly and that fails builds with modern IntelLLVM.
      # https://learn.microsoft.com/en-us/cpp/build/reference/compiler-option-warning-level
      target_compile_options(${t} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/WX->)
    else()
      # necessary to avoid
      # error: unknown warning option '-Wno-implicit-float-size-conversion' [-Werror,-Wunknown-warning-option]
      target_compile_options(${t} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:-Wno-error=unknown-warning-option;-Wno-error=character-conversion>")
    endif()
  endforeach()
endif()

include(GoogleTest)

# When cross-compiling without a target runner (emulator/device), gtest_discover_tests
# executes the test binary on the host, which fails for e.g. Android NDK builds.
# Set CROSSCOMPILING_EMULATOR_AVAILABLE=ON to opt into gtest_discover_tests even when
# CMAKE_CROSSCOMPILING is true (e.g. when an adb wrapper is configured as the runner).
if(CMAKE_CROSSCOMPILING)
  option(CROSSCOMPILING_EMULATOR_AVAILABLE
    "Target runner/emulator is available; use gtest_discover_tests even when cross-compiling" OFF)
else()
  set(CROSSCOMPILING_EMULATOR_AVAILABLE ON)
endif()

# Helper macro: use gtest_discover_tests on native builds or when a runner is available,
# fall back to gtest_add_tests (source-parsing, no host execution) otherwise.
# Skip code 77 is reserved by adb_run.sh for "no device/emulator" cases.
macro(ffs_gtest_register target_name)
  if(CROSSCOMPILING_EMULATOR_AVAILABLE)
    gtest_discover_tests(${target_name} ${ARGN} TEST_LIST _ffs_gtest_list)
  else()
    gtest_add_tests(TARGET ${target_name} TEST_LIST _ffs_gtest_list)
  endif()

  if(_ffs_gtest_list)
    set_tests_properties(${_ffs_gtest_list} PROPERTIES SKIP_RETURN_CODE 77)
  endif()
endmacro()


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
