#include <string>

#include "ffilesystem.h"

#if __has_include(<version>)
#include <version>
#endif


long fs_cpp_lang() {
  // C++ version compiler claims to support with given options
  return __cplusplus;
}


long fs_cpp_format() {
#ifdef __cpp_lib_format
  return __cpp_lib_format;
#else
  return 0;
#endif
}


long fs_cpp_ranges() {
#if defined(__cpp_lib_ranges) && defined(HAVE_CPP_RANGES)
  return __cpp_lib_ranges;
#else
  return 0;
#endif
}


std::string fs_backend() {
#ifdef HAVE_CXX_FILESYSTEM
  return "<filesystem>";
#else
  return "C";
#endif
}


bool fs_is_optimized() {
#if defined(__OPTIMIZE__) || defined(FFS_OPTIMIZED)
  return true;
#else
  return false;
#endif
}
