include(CheckCXXSymbolExists)

if(MSVC OR CYGWIN OR CMAKE_CROSSCOMPILING OR DEFINED ffilesystem_stdcpp_version)
  # excluding Cygwin due to glitch in try_run() where it can't delete the test executable after running it,
  # which causes the whole CMake configure to fail. This happened with an ARM64 CPU.
  return()
endif()

message(CHECK_START "Checking C++ standard library version")
# Intel, IntelLLVM and NVHPC on Linux use GNU libstdc++, so we need to extract the libstdc++ version
try_run(ffilesystem_stdcpp_run ffilesystem_stdcpp_build_ok
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/libstdcpp_version.cpp
  RUN_OUTPUT_STDOUT_VARIABLE _stdcpp_version
  RUN_OUTPUT_STDERR_VARIABLE _stdcpp_error
)

if(NOT ffilesystem_stdcpp_run EQUAL 0)
  message(CHECK_FAIL "Could not determine C++ STL version ${ffilesystem_stdcpp_build_ok} ${ffilesystem_stdcpp_run}
  ${_stdcpp_version}
  ${_stdcpp_error}")
else()
  string(STRIP "${_stdcpp_version}" _stdcpp_version)
  set(ffilesystem_stdcpp_version "${_stdcpp_version}" CACHE STRING "C++ standard library version")
  message(CHECK_PASS "${ffilesystem_stdcpp_version}")
endif()


if(ffilesystem_stdcpp_run EQUAL 0)
  if(ffilesystem_stdcpp_version MATCHES "GNU ([0-9]+)")
    if(CMAKE_MATCH_1 LESS 9)
      set(GNU_stdfs stdc++fs stdc++)
    endif()
  else()
    message(VERBOSE "libstdc++ version ${ffilesystem_stdcpp_version}")
  endif()
endif()

if(GNU_stdfs)
  set(CMAKE_REQUIRED_LIBRARIES ${GNU_stdfs})
  message(STATUS "adding C++ library flags ${GNU_stdfs}")
endif()

# libc++ hardening
# Detect libc++ being used: https://stackoverflow.com/a/78242553
# <version> is the preferred header (C++20+); fall back to <ciso646>
if(cxx_std_20 IN_LIST CMAKE_CXX_COMPILE_FEATURES)
  set(_stdlib_header version)
else()
  set(_stdlib_header ciso646)
endif()

check_cxx_symbol_exists(_LIBCPP_VERSION "${_stdlib_header}" ffilesystem_HAVE_LIBCPP)

if(ffilesystem_HAVE_LIBCPP)
  foreach(t IN ITEMS ffilesystem fs_cli filesystem_cli)
    if(TARGET ${t})
      target_compile_definitions(${t} PRIVATE _LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_FAST)
    endif()
  endforeach()
endif()
