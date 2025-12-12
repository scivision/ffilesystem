if(NOT MSVC AND NOT CMAKE_CROSSCOMPILING AND NOT DEFINED ffilesystem_stdcpp_version)

message(CHECK_START "Checking C++ standard library version")
# Intel, IntelLLVM and NVHPC on Linux use GNU libstdc++, so we need to extract the libstdc++ version
try_run(ffilesystem_stdcpp_run ffilesystem_stdcpp_build_ok
  ${CMAKE_CURRENT_BINARY_DIR}/libstdcpp_version
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/libstdcpp_version.cpp
  RUN_OUTPUT_VARIABLE _stdcpp_version
)

if(NOT ffilesystem_stdcpp_run EQUAL 0)
  message(CHECK_FAIL "Could not determine C++ STL version ${ffilesystem_stdcpp_build_ok} ${ffilesystem_stdcpp_run} ${_stdcpp_version}")
else()
  string(STRIP "${_stdcpp_version}" _stdcpp_version)
  set(ffilesystem_stdcpp_version "${_stdcpp_version}" CACHE STRING "C++ standard library version")
  message(CHECK_PASS "${ffilesystem_stdcpp_version}")
endif()

endif()

if(ffilesystem_stdcpp_run EQUAL 0)
  if(ffilesystem_stdcpp_version MATCHES "GNU ([0-9]+)")
    if(CMAKE_MATCH_1 LESS 9)
      set(GNU_stdfs stdc++fs stdc++)
    else()
      message(VERBOSE "GNU libstdc++ ${ffilesystem_stdcpp_version} is new enough to not need -lstdc++")
    endif()
  else()
    message(VERBOSE "Did not determine GNU libstdc++ version ${ffilesystem_stdcpp_version}")
  endif()
endif()

if(GNU_stdfs)
  set(CMAKE_REQUIRED_LIBRARIES ${GNU_stdfs})
  message(STATUS "adding C++ library flags ${GNU_stdfs}")
endif()

# libc++ hardening
if(ffilesystem_stdcpp_version MATCHES "^LLVM")
  add_compile_definitions(_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_FAST)
endif()
