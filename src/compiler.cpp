#include <string>

#if __has_include(<format>)
#include <format> // IWYU pragma: keep
#endif

// libcxx_release
#if __has_include(<version>)
#include <version>
#elif __has_include(<ciso646>)
// < C++20 standard
#include <ciso646>
#endif

// libc_release
#if defined(__GLIBC__)
#include <gnu/libc-version.h>
#endif

#include "ffilesystem.h"


std::string fs_libc()
{
  return
#if defined(__GLIBC__)
  "GCC: runtime " + std::string(gnu_get_libc_version()) + ", compile-time " + std::to_string(__GLIBC__) + "." + std::to_string(__GLIBC_MINOR__);
#elif defined(__apple_build_version__)
  "Apple";
#elif defined(_MSC_VER)
  "Microsoft " + std::to_string(_MSC_VER);
#else
  "";
#endif
}

namespace ffilesystem {

constexpr unsigned long libcxx_release =
#if defined(_LIBCPP_VERSION)  // LLVM libc++
  _LIBCPP_VERSION;
#elif defined(_GLIBCXX_RELEASE)  // GNU libstdc++
  _GLIBCXX_RELEASE;
#elif defined(_MSVC_STL_UPDATE)  // Microsoft STL
  _MSVC_STL_UPDATE;
#else
  0;
#endif

constexpr std::string_view libcxx_name =
#if defined(_LIBCPP_VERSION)
  "LLVM libc++";
#elif defined(_GLIBCXX_RELEASE)
  "GNU libstdc++";
#elif defined(_MSVC_STL_UPDATE)
  // https://github.com/microsoft/STL/wiki/Macro-_MSVC_STL_UPDATE
  "Microsoft STL";
#endif

std::string libcxx(){
  return std::string(libcxx_name) + " " + std::to_string(libcxx_release);
}


std::string compiler()
{

  return
#if defined(__INTEL_LLVM_COMPILER)

#ifdef __cpp_lib_format // C++20
  std::format("{} {}", __VERSION__, __INTEL_LLVM_COMPILER);
#else
  std::string(__VERSION__) + " " + std::to_string(__INTEL_LLVM_COMPILER);
#endif

#elif defined(__NVCOMPILER_LLVM__)

// not __VERSION__ because it emits "EDG g++ mode" instead of "NVIDIA"

#ifdef __cpp_lib_format
  std::format("NVIDIA {}.{}.{}", __NVCOMPILER_MAJOR__, __NVCOMPILER_MINOR__, __NVCOMPILER_PATCHLEVEL__);
#else
  "NVIDIA " + std::to_string(__NVCOMPILER_MAJOR__) + "." + std::to_string(__NVCOMPILER_MINOR__) + "." + std::to_string(__NVCOMPILER_PATCHLEVEL__);
#endif

#elif defined(__clang__)

# ifdef __VERSION__
  __VERSION__;
# elif defined(__cpp_lib_format)
  std::format("Clang {}.{}.{}", __clang_major__, __clang_minor__, __clang_patchlevel__);
# else
  "Clang " + std::to_string(__clang_major__) + "." + std::to_string(__clang_minor__) + "." + std::to_string(__clang_patchlevel__);
# endif

#elif defined(__GNUC__)

# ifdef __cpp_lib_format
  std::format("GNU GCC {}.{}.{}", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
# else
  "GNU GCC " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__) + "." + std::to_string(__GNUC_PATCHLEVEL__);
# endif

#elif defined(_MSC_FULL_VER)
  "MSVC " + std::to_string(_MSC_FULL_VER);
#endif
}

}
