# Fast-filesystem: platform-independent, compiler-agnostic path manipulation and filesystem library

[![DOI](https://zenodo.org/badge/433875623.svg)](https://zenodo.org/badge/latestdoi/433875623)
[![ci](https://github.com/scivision/fast-filesystem/actions/workflows/ci.yml/badge.svg)](https://github.com/scivision/fast-filesystem/actions/workflows/ci.yml)
[![ci_windows](https://github.com/scivision/fast-filesystem/actions/workflows/ci_windows.yml/badge.svg)](https://github.com/scivision/fast-filesystem/actions/workflows/ci_windows.yml)
[![oneapi-linux](https://github.com/scivision/fast-filesystem/actions/workflows/oneapi-linux.yml/badge.svg)](https://github.com/scivision/fast-filesystem/actions/workflows/oneapi-linux.yml)
[![ci_fpm](https://github.com/scivision/fast-filesystem/actions/workflows/ci_fpm.yml/badge.svg)](https://github.com/scivision/fast-filesystem/actions/workflows/ci_fpm.yml)
[![ci_meson](https://github.com/scivision/fast-filesystem/actions/workflows/ci_meson.yml/badge.svg)](https://github.com/scivision/fast-filesystem/actions/workflows/ci_meson.yml)

Platform independent (Linux, macOS, Windows, Cygwin, WSL, BSD, ...) and compiler-agnostic Fast-filesystem "Ffilesystem" path manipulation library.
Simplicity and efficiency are focuses of Ffilesystem.
Ffilesystem backend is implemented in C++17 using `<string_view>` for simplicity and speed.
If available,
[C++ standard library `<filesystem>`](https://en.cppreference.com/w/cpp/filesystem)
is used.
The C++ backend accesses the
[C standard library](https://en.wikipedia.org/wiki/C_standard_library)
to access filesystem and system parameters.
Ffilesystem uses C++ `std::string` and (optional) Fortran `character`.
[UTF-8 encoding](https://utf8everywhere.org/)
allows for multibyte characters in paths.
Ffilesystem does not throw or catch C++ exceptions, although the underlying C++ STL may.

Ffilesystem header
[ffilesystem.h](./include/ffilesystem.h)
can be used from C and C++ project code--see
[example](./example).
The C interface allows reuse of Ffilesystem functions in other code languages such as
[Matlab](./example/matlab_ffilesystem.m).

The optional Fortran interface is built by default.
Ffilesystem brings full, fast filesystem functionality to Fortran.

The language standards must be at least:

* C++17 standard library [STL](./Readme_cpp_stl.md)
* (optional) Fortran 2003

Ffilesystem works with popular C++ STL and C standard library implementations including:
[glibc](https://sourceware.org/glibc/),
[newlib](https://sourceware.org/newlib/),
[musl](https://musl.libc.org/),
[Cosmopolitan universal binaries](./Readme_cosmopolitan.md),
[macOS universal binaries](./Readme_macos.md),
BSD libc,
[Microsoft CRT](https://en.wikipedia.org/wiki/Microsoft_Windows_library_files#CRT),
among others.
On Linux, symbol
[_DEFAULT_SOURCE](https://man7.org/linux/man-pages/man7/feature_test_macros.7.html)
is defined if needed to enable C standard library functions.

Inspired by (and benchmarked against)
[Python pathlib](https://docs.python.org/3/library/pathlib.html).
Important Ffilesystem functions are
[benchmarked](./benchmark.md)
to help improve performance.
Advanced / conceptual development takes place in
[ffilesystem-concepts](https://github.com/scivision/ffilesystem-concepts).

## Compiler support

Ffilesystem supports compilers including:

* GCC &ge; 7 (gcc/g++, gfortran)
* LLVM Clang &ge; 9 (clang/clang++, flang or gfortran)
* Intel oneAPI &ge; 2023.1 (icx, icpx, ifx)
* Intel Classic &ge; 2021.9 (icpc, ifort)
* AMD AOCC (clang/clang++, flang)
* NVIDIA HPC SDK (nvc++, nvfortran)
* Visual Studio (C/C++)
* Cray: using Cray compilers alone (cc, CC, ftn) or using GCC or Intel backend

To help debug with older compilers, disable C++ `<filesystem>`:

```sh
cmake -Bbuild -Dffilesystem_cpp=off
```

### libstdc++

Some systems have broken, obsolete, or incompatible libstdc++.

**Intel**: If Intel compiler linker errors use GCC >= 9.1.
This can be done by setting environment variable CXXFLAGS to the top level GCC >= 9.1 directory.
Set environment variable CXXFLAGS for
[Intel GCC toolchain](https://www.intel.com/content/www/us/en/develop/documentation/oneapi-dpcpp-cpp-compiler-dev-guide-and-reference/top/compiler-reference/compiler-options/compiler-option-details/compatibility-options/gcc-toolchain.html)
like:

```sh
export CXXFLAGS=--gcc-toolchain=/opt/rh/gcc-toolset-10/root/usr/
```

which can be determined like:

```sh
scl enable gcc-toolset-10 "which g++"
```

## Build

Ffilesystem can be built with CMake, Meson, or Fortran Package Manager (FPM).

"libffilesystem.a" is the library binary built that contains the Fortran "filesystem" module--it is the only binary you need to use in your project.

Please see the [API docs](./API.md) for extensive list of functions/subroutines.

Use any one of these methods to build Ffilesystem:

CMake:

```sh
cmake -B build
cmake --build build
# optional
ctest --test-dir build
```

The default library with CMake is static; to build shared library:

```sh
cmake -B build -DBUILD_SHARED_LIBS=on
...
```

---

Meson:

```sh
meson setup build
meson compile -C build
# optional
meson test -C build
```

The default library with Meson is shared; to build static library:

```sh
meson setup -Ddefault_library=static build
...
```

---

Fortran Package Manager (FPM):

```sh
fpm --cxx-flag=-std=c++17 build
# c++17 is the minimum, can use newer
```

---

GNU Make:

```sh
make
```

We provide Fortran REPL "filesystem_cli" and C++ REPL "fs_cli" for interactive testing of Ffilesystem routines.

### Build options

Fortran "filesystem" module contains OPTIONAL (enabled by default) Fortran type "path_t" that contains properties and methods.
The "path_t" type uses getter and setter procedure to access the path as a string `character(:), allocatable`.

```fortran
use filesystem, only : path_t

type(path_t) :: p

p = path_t("my/path")  !< setter

print *, "path: ", p%path() !< getter
```

The CMake and Meson scripts detect if Fortran 2003 `type` is available and enable `path_t` by default.
To manually enable / disable `path_t` with CMake set command option `cmake -DHAVE_F03TYPE=1` or `cmake -DHAVE_F03TYPE=0` respectively.

By default `stat()` is used on non-Windows systems to get file information.
On glibc systems,
[statx()](https://www.man7.org/linux/man-pages/man2/statx.2.html)
may be used if available by setting build option

```sh
cmake -Dffilesystem_statx=true -Bbuild

# or

meson setup -Dstatx=true build
```

## Self test

The optional self-tests provide reasonable coverage of the Ffilesystem library.
Several of the tests use `argv[0]` as a test file.
We are aware of the shortcomings of `argv[0]` to get the executable name.
We provide the function `fs_exepath()` to get the executable path reliably.

## Usage from other projects

The [example](./example) directory contains a use pattern from external projects.
One can either `cmake --install build` ffilesystem or use CMake ExternalProject or
[FetchContent](https://gist.github.com/scivision/245a1f32498d15a87a507051857327d9)
from the other project.
To find ffilesystem in your CMake project:

```cmake
find_package(ffilesystem CONFIG REQUIRED)
```

CMake package variables `ffilesystem_cpp` and `ffilesystem_fortran` indicate whether ffilesystem was built with C++ `<filesystem>` and/or Fortran support.

[ffilesystem.cmake](./cmake/ffilesystem.cmake) would be included from the other project to find or build Ffilesystem automatically.
It provides the appropriate imported targets for shared or static builds, including Windows DLL handling.

## Notes

GCC 6.x and older aren't supported due to lack of C++17 string_view support.

### Possible future features

Use statx() if available to inquire if a file is encrypted or compressed, etc.

### Other C++ filesystem libraries

Ffilesystem emphasizes simplicity and reasonable performance and reliability for scientific computing, particularly on HPC systems.
A highly performance-oriented C++ low-level no TOCTOU filesystem library is
[LLFIO](https://github.com/ned14/llfio).

Other implementations of C++ filesystem include:

* [Boost.Filesystem](https://www.boost.org/doc/libs/1_86_0/libs/filesystem/doc/index.htm) what the stdlib filesystem is based on, often tries newer features. [Boost.Filesystem source code](https://github.com/boostorg/filesystem)
* [ghc-filesystem](https://github.com/gulrak/filesystem) for older compilers.
* deprecated `<experimental/filesystem>` is missing vital lexical operations.

### Other Fortran filesystem libraries

Other Fortran libraries that provide interfaces to filesystems include the following.
Generally they have noticeably fewer functions than Ffilesystem.
They typically implement many functions in Fortran, where with Ffilesystem we implement in C++ or C++ `<filesystem>` if available.
Ffilesystem Fortran code is optional, and is just a thin wrapper around the C++ functions.

* [stdlib_os](https://github.com/MarDiehl/stdlib_os)
* [fortyxima](https://bitbucket.org/aradi/fortyxima/src/develop/)
* [Fortran-stdlib](https://github.com/fortran-lang/stdlib/issues/201)
* [M_system](https://github.com/urbanjost/M_system) focuses on interfaces to libc

---

There is no "is_musl()" function due to MUSL designers
[not providing](https://stackoverflow.com/questions/58177815/how-to-actually-detect-musl-libc)
a
[MUSL feature macro](https://wiki.musl-libc.org/faq.html).

### Windows

Like
[Microsoft STL](https://github.com/microsoft/STL/issues/2256),
Ffilesystem is not designed for UNC "long" paths.
We recommend using a UNC path to a mapped drive letter.
Windows
[long paths](https://github.com/microsoft/STL/issues/1921)
are not implemented due to
[limitations](https://www.boost.org/doc/libs/1_86_0/libs/filesystem/doc/reference.html#windows-path-prefixes).

Enable Windows
[developer mode](https://learn.microsoft.com/en-us/windows/apps/get-started/developer-mode-features-and-debugging)
to use symbolic links if needed.

### C++ filesystem discussion

Security
[research](https://www.reddit.com/r/cpp/comments/151cnlc/a_safety_culture_and_c_we_need_to_talk_about/?rdt=62365)
led to
[TOCTOU](https://en.wikipedia.org/wiki/Time-of-check_to_time-of-use)-related
patches to the C++ filesystem library in various C++ standard library implementations noted in that discussion.
Ffilesystem does NOT use remove_all, which was the TOCTOU concern addressed above.

Since the underlying C++ filesystem is not thread-safe, race conditions can occur if multiple threads are accessing the same filesystem object regardless of the code language used in the Ffilesystem library.
