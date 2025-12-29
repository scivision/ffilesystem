# ref: https://jcbhmr.me/blog/cosmocc-cmake
# https://gitlab.kitware.com/cmake/cmake/-/issues/25578
#
# Usage:
# cmake -B build-cosmo --toolchain cosmopolitan.cmake -DCOSMO_ROOT=$HOME/cosmo -Dffilesystem_fortran=off
# cmake --build build-cosmo

set(CMAKE_SYSTEM_NAME Generic)
unset(CMAKE_SYSTEM_PROCESSOR)

find_program(CMAKE_ASM_COMPILER cosmocc REQUIRED PATH_SUFFIXES bin HINTS ${COSMO_ROOT})
find_program(CMAKE_C_COMPILER cosmocc REQUIRED PATH_SUFFIXES bin HINTS ${COSMO_ROOT})
find_program(CMAKE_CXX_COMPILER cosmoc++ REQUIRED PATH_SUFFIXES bin HINTS ${COSMO_ROOT})

find_program(CMAKE_AR
NAMES cosmoar
REQUIRED
PATH_SUFFIXES bin
HINTS ${COSMO_ROOT}
)
find_program(CMAKE_RANLIB
NAMES cosmoranlib
REQUIRED
PATH_SUFFIXES bin
HINTS ${COSMO_ROOT}
 )

set(CMAKE_CXX_FLAGS_INIT "-fexceptions -frtti")

set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_LIST_DIR}/cmake/cosmocc-override.cmake)

set(COSMOPOLITAN 1)
set(UNIX 1)

# Cosmopolitan uses only static libraries and no RPATH
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_SKIP_RPATH ON)

# The applications can run on the host platform
set(CMAKE_CROSSCOMPILING OFF)

set(ffilesystem_fortran false)
