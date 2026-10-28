#include "ffilesystem.h"
#include <string_view>

#include <boost/ut.hpp>

// /dev/stdin may not be available on CI systems

int main() {
  using namespace boost::ut;

  "is_char"_test = [] {
    if (ffilesystem::is_windows) {
      expect(ffilesystem::is_char_device("NUL"));
      expect(ffilesystem::is_char_device("CONIN$"));
    } else {
      expect(ffilesystem::is_char_device("/dev/null"));
    }
  };

  "is_file"_test = [] {
    if (ffilesystem::is_windows) {
      expect(!ffilesystem::is_file("NUL"));
      expect(!ffilesystem::is_file("CONIN$"));
    } else {
      expect(!ffilesystem::is_file("/dev/null"));
    }
  };

if (ffilesystem::backend == "<filesystem>" && fs_is_mingw() && ffilesystem::compiler().substr(0, 5) == "Clang")
  {
    skip / "exists"_test = [] {};
  } else {
  "exists"_test = [] {
    if (ffilesystem::is_windows) {
      expect(ffilesystem::exists("NUL"));
      expect(ffilesystem::exists("CONIN$"));
    } else {
      expect(ffilesystem::exists("/dev/null"));
    }
  };
}
}
