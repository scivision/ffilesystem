include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckCXXSymbolExists)
include(CheckSourceCompiles)

# --- some compilers require these manual settings
unset(CMAKE_REQUIRED_FLAGS)
unset(CMAKE_REQUIRED_LIBRARIES)
unset(CMAKE_REQUIRED_DEFINITIONS)
unset(GNU_stdfs)

if(CMAKE_VERSION VERSION_LESS 3.25 AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(LINUX true)
endif()

# --- compiler standard setting -- need if(NOT) in case CMAKE_CXX_STANDARD is set but blank.
if(NOT CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 17)
  if(WIN32)
    # we assume Windows has modern compilers by default
    set(CMAKE_CXX_STANDARD 20)
  else()
    set(CMAKE_CXX_STANDARD 17)
  endif()
endif()


if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24 AND CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND CMAKE_GENERATOR STREQUAL "Unix Makefiles")
  # otherwise failed to link since -lc++ is missing
  set(ffilesystem_linker_lang CXX)
else()
  # IntelLLVM|NVHPC need Fortran.
  # For other compilers (except as above) don't need it set, but Fortran doesn't hurt.
  set(ffilesystem_linker_lang Fortran)
endif()


if(NOT MSVC AND NOT DEFINED ffilesystem_stdcpp_version)

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

if(ffilesystem_cpp)
  include(${CMAKE_CURRENT_LIST_DIR}/CppCheck.cmake)
  cpp_check()
else()
  unset(HAVE_CXX_FILESYSTEM CACHE)
endif()

if(UNIX AND BUILD_SHARED_LIBS)
  list(APPEND CMAKE_REQUIRED_LIBRARIES ${CMAKE_DL_LIBS})

  set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
  check_cxx_symbol_exists(dladdr "dlfcn.h" ffilesystem_HAVE_DLADDR)

  unset(CMAKE_REQUIRED_DEFINITIONS)
endif()

# --- deeper filesystem check: C, C++ and Fortran compiler ABI compatibility

if(HAVE_CXX_FILESYSTEM)

if(ffilesystem_trace)
  # this is not on by default because it gave a false negative on some Cray systems
  # while bypassing it let all of Ffilesystem work.
  include(${CMAKE_CURRENT_LIST_DIR}/FScheck.cmake)
  fs_check()

endif()

elseif(UNIX AND NOT APPLE)

  set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
  check_symbol_exists(copy_file_range "unistd.h" ffilesystem_HAVE_COPY_FILE_RANGE)

endif()

if(ffilesystem_cpp AND NOT ffilesystem_fallback AND NOT HAVE_CXX_FILESYSTEM)
  message(FATAL_ERROR "C++ filesystem not available. To fallback to plain C++:
  cmake -Dffilesystem_fallback=on -B build"
  )
endif()

# fixes errors about needing -fPIE
if(LINUX)
  include(CheckPIESupported)
  check_pie_supported()
  set(CMAKE_POSITION_INDEPENDENT_CODE true)
endif()

if(HAVE_Fortran_FILESYSTEM AND NOT DEFINED HAVE_F03TYPE)
  if(ffilesystem_trace)
    include(${CMAKE_CURRENT_LIST_DIR}/FortranCheck.cmake)
    fortran_check()
  else()
    set(HAVE_F03TYPE true)
  endif()
endif()

# --- END COMPILER CHECKS


# --- C / C++ compile flags
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU|^Intel")
  add_compile_options(
  "$<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<CONFIG:Debug>>:-Wextra>"
  "$<$<COMPILE_LANGUAGE:C,CXX>:-Wall>"
  "$<$<COMPILE_LANGUAGE:C>:-Werror=implicit-function-declaration>"
  )
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
  add_compile_options("$<$<COMPILE_LANGUAGE:C,CXX>:/W3;/wd4996>")
endif()

add_compile_options("$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang,GNU>:-Wold-style-cast>")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.14)
  # MSVC has __cpluscplus = 199711L by default, which is C++98!
  # MSVC 15.7 (19.14) added /Zc:__cplusplus to set __cplusplus to the true value.
  # oneAPI since 2023.1 sets __cplusplus to the true value with MSVC by auto-setting this flag.
  add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>")
endif()

# too many false positives esp. regarding argv[1] and higher
# add_compile_options("$<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang,IntelLLVM>:-Wunsafe-buffer-usage>")


if(CMAKE_C_COMPILER_ID STREQUAL "IntelLLVM")
  add_compile_options("$<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<CONFIG:Debug>>:-Rno-debug-disables-optimization>")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "10")
  add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-Wno-attributes>")
  # this is for UNLIKELY/LIKELY macros
endif()

# --- Fortran compile flags
if(CMAKE_Fortran_COMPILER_ID MATCHES "^Intel")

add_compile_options(
"$<$<COMPILE_LANGUAGE:Fortran>:-warn>"
"$<$<AND:$<COMPILE_LANGUAGE:Fortran>,$<CONFIG:Debug>>:-traceback;-check;-debug>"
)

# this flag needs to be applied EVERYWHERE incl. submodule projects
# or runtime errors / weird behavior with unresolved procedures that actually exist.
# -standard-semantics is no good because it breaks linkage within oneAPI itself e.g. oneMPI library!
if(WIN32)
  add_compile_options("$<$<COMPILE_LANGUAGE:Fortran>:/fpscomp:logicals>")
else()
  add_compile_options("$<$<COMPILE_LANGUAGE:Fortran>:-fpscomp;logicals>")
endif()

elseif(CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")

add_compile_options(
"$<$<AND:$<COMPILE_LANGUAGE:Fortran>,$<CONFIG:Debug>>:-Wextra>"
"$<$<COMPILE_LANGUAGE:Fortran>:-Wall;-fimplicit-none>"
"$<$<AND:$<COMPILE_LANGUAGE:Fortran>,$<CONFIG:Debug>>:-fcheck=all;-Werror=array-bounds>"
"$<$<AND:$<COMPILE_LANGUAGE:Fortran>,$<NOT:$<CONFIG:Debug>>>:-fno-backtrace>"
)

elseif(CMAKE_Fortran_COMPILER_ID STREQUAL "NVHPC")
  # C_BOOL correctness
  add_compile_options("$<$<COMPILE_LANGUAGE:Fortran>:-Munixlogical>")
endif()

# --- code coverage
if(ffilesystem_coverage)
  include(CodeCoverage)
  append_coverage_compiler_flags()
  set(COVERAGE_EXCLUDES ${PROJECT_SOURCE_DIR}/src/tests)
endif()

# --- clang-tidy
if(ffilesystem_tidy)
  find_program(CLANG_TIDY_EXE NAMES clang-tidy REQUIRED
  PATHS /opt/homebrew/opt/llvm/bin
  )
  set(tidy_cmd ${CLANG_TIDY_EXE} -format-style=file)
  set(CMAKE_C_CLANG_TIDY ${tidy_cmd})
  set(CMAKE_CXX_CLANG_TIDY ${tidy_cmd})
endif()

# --- IWYU
if(ffilesystem_iwyu)
  find_program(IWYU_EXE NAMES include-what-you-use REQUIRED)
  message(STATUS "IWYU_EXE: ${IWYU_EXE}")
  set(iwyu_cmd ${IWYU_EXE})
  set(CMAKE_C_INCLUDE_WHAT_YOU_USE ${iwyu_cmd})
  set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${iwyu_cmd})
endif()
