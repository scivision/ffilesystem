#include "ffilesystem.h"
#include <string_view>

#include <boost/ut.hpp>

// /dev/stdin may not be available on CI systems

int main() {
  using namespace boost::ut;

  "is_char"_test = [] {
    if (fs_is_windows()) {
      expect(fs_is_char_device("NUL"));
      expect(fs_is_char_device("CONIN$"));
    } else {
      expect(fs_is_char_device("/dev/null"));
    }
  };

  "is_file"_test = [] {
    if (fs_is_windows()) {
      expect(!fs_is_file("NUL"));
      expect(!fs_is_file("CONIN$"));
    } else {
      expect(!fs_is_file("/dev/null"));
    }
  };

  "exists"_test = [] {
    if (fs_is_windows()) {
      if (fs_backend() == "<filesystem>" && fs_is_mingw() && fs_compiler().substr(0, 5) == "Clang") {
        return;
      }

      expect(fs_exists("NUL"));
      expect(fs_exists("CONIN$"));
    } else {
      expect(fs_exists("/dev/null"));
    }
  };
}
