cmake_minimum_required(VERSION 3.20...4.01)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "Please use out of source build like:
  cmake -Bbuild")
endif()

project(ffilesystem
LANGUAGES C CXX
VERSION 6.4.0
)

include(GNUInstallDirs)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
# This project isn't using C++ modules; avoid bugs in module scanning CMake 3.28, 3.29 with GCC >= 14.

include(options.cmake)

if(ffilesystem_fortran)
    enable_language(Fortran)
    set(HAVE_Fortran_FILESYSTEM ON)
else()
    set(HAVE_Fortran_FILESYSTEM OFF)
endif()

include(cmake/compilers.cmake)

message(STATUS "${PROJECT_NAME} ${PROJECT_VERSION} C++${CMAKE_CXX_STANDARD} CMake ${CMAKE_VERSION} ${CMAKE_SYSTEM_NAME} Toolchain ${CMAKE_TOOLCHAIN_FILE}")
message(VERBOSE
"CMAKE_AR ${CMAKE_AR}
CMAKE_RANLIB ${CMAKE_RANLIB}
CMAKE_STRIP ${CMAKE_STRIP}"
)
message(VERBOSE
"CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}
CMAKE_CXX_COMPILER_LINKER_ID: ${CMAKE_CXX_COMPILER_LINKER_ID}
CMAKE_CXX_COMPILER_LINKER_ID: ${CMAKE_CXX_COMPILER_LINKER_ID}
CMAKE_Fortran_COMPILER_LINKER_ID: ${CMAKE_Fortran_COMPILER_LINKER_ID}"
)
if(ffilesystem_stdcpp_version)
  message(STATUS "C++ standard library: ${ffilesystem_stdcpp_version}")
endif()

# CPPcheck
if(ffilesystem_cppcheck)
  find_program(cppcheck_exe NAMES cppcheck REQUIRED)
  set(cppcheck_opts --enable=all --inline-suppr --quiet --suppressions-list=${PROJECT_SOURCE_DIR}/cppcheck.supp)
  set(CMAKE_C_CPPCHECK ${cppcheck_exe} --std=c++${CMAKE_CXX_STANDARD} ${cppcheck_opts})
  set(CMAKE_CXX_CPPCHECK ${cppcheck_exe} --std=c++${CMAKE_CXX_STANDARD} ${cppcheck_opts})
endif()

# --- filesystem library

# use separate object for Fortran to work with Visual Studio generator
if(HAVE_Fortran_FILESYSTEM)

  add_library(ffilesystem_Fortran OBJECT src/fortran/filesystem.F90)

  target_compile_definitions(ffilesystem_Fortran PRIVATE
  $<$<NOT:$<BOOL:${HAVE_F03TYPE}>>:NO_F03TYPE>
  )
  set_property(TARGET ffilesystem_Fortran PROPERTY
  Fortran_MODULE_DIRECTORY ${PROJECT_BINARY_DIR}/include
  )

endif()

set(_src
src/c.cpp
src/absolute.cpp
src/copy.cpp
src/extra/compiler.cpp
src/extra/cygwin.cpp
src/equivalent.cpp
src/env.cpp
src/executable.cpp
src/home.cpp
src/inquire.cpp
src/lang.cpp
src/lexical.cpp
src/limits.cpp
src/extra/locale.cpp
src/log.cpp
src/mkdir.cpp
src/move.cpp
src/normalize.cpp
src/os.c
src/extra/owner.cpp
src/parent.cpp
src/partition.cpp
src/permissions.cpp
src/platform.cpp
src/pure.cpp
src/relative.cpp
src/resolve.cpp
src/size.cpp
src/space.cpp
src/symlink.cpp
src/tempdir.cpp
src/touch.cpp
src/time.cpp
src/which.cpp
src/windows.cpp
)

if(ffilesystem_extra)
  list(APPEND _src
src/extra/case.cpp
src/extra/component.cpp
src/extra/exepath.cpp
src/extra/libpath.cpp
src/extra/random.cpp
src/extra/shell.cpp
src/extra/sysctl.cpp
src/extra/uid.cpp
src/extra/uname.cpp
src/extra/winsock.cpp
  )
endif()

if(TARGET ffilesystem_Fortran)
  list(APPEND _src $<TARGET_OBJECTS:ffilesystem_Fortran>)
endif()

add_library(ffilesystem ${_src})

target_include_directories(ffilesystem PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_compile_definitions(ffilesystem PRIVATE
"$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<BOOL:${HAVE_CXX_FILESYSTEM}>>:HAVE_CXX_FILESYSTEM>"
"$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<BOOL:${ffilesystem_HAVE_CLOCK_CAST}>>:HAVE_CLOCK_CAST>"
"$<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<BOOL:${ffilesystem_trace}>>:FS_TRACE>"
"$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<BOOL:${ffilesystem_HAVE_DLADDR}>>:HAVE_DLADDR>"
"$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<BOOL:${ffilesystem_HAVE_COPY_FILE_RANGE}>>:HAVE_COPY_FILE_RANGE>"
"$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<BOOL:${ffilesystem_statx}>>:USE_STATX>"
"$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<BOOL:${ffilesystem_locale}>>:ffilesystem_locale>"
"$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<BOOL:${ffilesystem_extra}>>:ffilesystem_extra>"
)

target_compile_features(ffilesystem PRIVATE
$<$<BOOL:${CMAKE_CXX_STANDARD}>:cxx_std_${CMAKE_CXX_STANDARD}>
c_std_99
)

set_target_properties(ffilesystem PROPERTIES
PUBLIC_HEADER include/ffilesystem.h
EXPORT_NAME filesystem
VERSION ${PROJECT_VERSION}
WINDOWS_EXPORT_ALL_SYMBOLS ON
)
if(WIN32 OR CYGWIN)
  target_compile_definitions(ffilesystem PRIVATE $<$<COMPILE_LANGUAGE:CXX>:FS_DLL_NAME="$<TARGET_FILE_NAME:ffilesystem>">)
endif()

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/include)

target_include_directories(ffilesystem PUBLIC
"$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include;${PROJECT_SOURCE_DIR}/include>"
$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(ffilesystem PUBLIC
${GNU_stdfs}
${CMAKE_DL_LIBS}
"$<$<BOOL:${WIN32}>:Secur32;Ole32;Shell32;Shlwapi;uuid;Userenv;Advapi32;ws2_32>"
)
# Secur32: GetUserNameExW
# both Advapi32 and Userenv must be linked
# Ole32: CoTaskMemFree
# Shell32: SHGetKnownFolderPath
# uuid: KnownFolderID

get_property(_ffs_defs TARGET ffilesystem PROPERTY COMPILE_DEFINITIONS)
message(VERBOSE "Ffilesystem COMPILE_DEFINITIONS: ${_ffs_defs}")
get_property(_ffs_flags TARGET ffilesystem PROPERTY COMPILE_OPTIONS)
message(VERBOSE "Ffilesystem COMPILE_OPTIONS: ${_ffs_flags}")
get_property(_ffs_libs TARGET ffilesystem PROPERTY LINK_LIBRARIES)
message(VERBOSE "Ffilesystem LINK_LIBRARIES: ${_ffs_libs}")

if(HAVE_Fortran_FILESYSTEM)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/filesystem.mod TYPE INCLUDE)
endif()

install(TARGETS ffilesystem EXPORT ${PROJECT_NAME}-targets)

# GLOBAL for use from FetchContent
add_library(ffilesystem::filesystem INTERFACE IMPORTED GLOBAL)

target_link_libraries(ffilesystem::filesystem INTERFACE ffilesystem)
set_property(TARGET ffilesystem::filesystem PROPERTY IMPORTED_LINK_INTERFACE_LANGUAGE CXX)
# https://cmake.org/cmake/help/latest/prop_tgt/IMPORTED_LINK_INTERFACE_LANGUAGES.html
# imported targets use above instead of LINKER_LANGUAGE
# target_link_libraries(ffilesystem::filesystem INTERFACE stdc++)  # did not help


# --- CLI

if(ffilesystem_cli)
  add_executable(ffilesystem_bench app/benchmark.cpp)
  target_link_libraries(ffilesystem_bench PRIVATE ffilesystem)

  add_executable(fs_cli app/main.cpp)
  target_link_libraries(fs_cli PRIVATE ffilesystem)
  target_compile_definitions(fs_cli PRIVATE
  "$<$<BOOL:${HAVE_CXX_FILESYSTEM}>:HAVE_CXX_FILESYSTEM>"
  "$<$<BOOL:${ffilesystem_extra}>:ffilesystem_extra>"
  )
  install(TARGETS fs_cli EXPORT ${PROJECT_NAME}-targets)

  if(CMAKE_CXX_STANDARD LESS 20 AND "cxx_std_20" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    target_compile_features(fs_cli PRIVATE cxx_std_20)
    target_compile_features(ffilesystem_bench PRIVATE cxx_std_20)
  endif()

  add_executable(libc_version app/libc.cpp)
  add_executable(libcpp_version app/libcxx.cpp)

  if(HAVE_Fortran_FILESYSTEM)
  add_executable(filesystem_cli app/fortran/main.f90)
  target_link_libraries(filesystem_cli PRIVATE ffilesystem)
  set_property(TARGET filesystem_cli PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  set_property(TARGET filesystem_cli PROPERTY LINKER_LANGUAGE ${ffilesystem_linker_lang})

  install(TARGETS filesystem_cli EXPORT ${PROJECT_NAME}-targets)
  endif()
endif()

# --- Tests
if(ffilesystem_BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

include(cmake/install.cmake)

include(FeatureSummary)

add_feature_info(Fortran HAVE_Fortran_FILESYSTEM "Fortran bindings")
add_feature_info(Cpp HAVE_CXX_FILESYSTEM "C++ stdlib filesystem")
add_feature_info(shared BUILD_SHARED_LIBS "Build shared library: enables lib_path()")
add_feature_info(path_t HAVE_F03TYPE "Fortran 2003 path_t object-oriented interface")

feature_summary(WHAT ENABLED_FEATURES DESCRIPTION "${PROJECT_NAME} enabled features:")
