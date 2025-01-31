#include <iostream>
#include <cstdlib>
#include <algorithm>  // for std::move
#include <string>
#include <vector>
#include <tuple>
#include <iterator> // for std::back_inserter

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main() {

int fail = 0;

// string_view wouldn't allow modifying MSVC-like compilers, so use string
std::vector<std::tuple<std::string, std::string, std::string>>
tests = {{"", "", "."},
    {"Hello", "Hello", "."},
    {"Hello", "Hello/", "."},
    {"a/b", "a/b", "."},
    {"a///b", "a/b", "."},
    {"a/b", "a///b", "."},
    {"a/b", "a/b/", "."},
    {"a/b/", "a/b", "."},
    {"a/b", "a", ".."},
    {"a/b", "a/", ".."},
    {"a", "a/b/", "b"},
    {"a", "a/b/.", "b"},
    {"a", "a/b/..", "."},
    {"a/b/c/d", "a/b", "../.."},
    {"a/b/c/d", "a/b/", "../.."},
    {"./a/b", "./a/c", "../c"},
};

std::vector<std::tuple<std::string, std::string, std::string>> tos;

if(fs_is_windows()){

  std::string c = fs_getenv("SYSTEMDRIVE");
  std::cout << "SYSTEMDRIVE: " << c << " length " << c.length() << "\n";

  if (c.length() == 2){
    tos = {
      {c+"/", c+"/a/b", "a/b"},
      {c+"/a/b", c+"/a/b", "."},
      {c+"/a/b", c+"/a", ".."},
      // {c+"/a", "b", "b"}  //  ambiguous with Clang/Flang ARM MinGW <filesystem>
    };
  } else {
    std::cerr << "Warning: SYSTEMDRIVE not set, skipping tests\n";
  }
// NOTE: on Windows, if a path is real, finalPath is used, which makes drive letters upper case.
// we didn't use a random drive letter because a removable drive at that letter can make the test fail spuriously.

} else {
  tos = {
    {"", "a", "a"},
    {"/", "/", "."},
    {"Hello", "Hello", "."},
    {"Hello", "Hello/", "."},
    {"/dev/null", "/dev/null", "."},
    {"a/b", "c", "../../c"},
    {"c", "a/b", "../a/b"},
    {"a/b", "a/c", "../c"}
  };
}
// NOTE: use relative non-existing paths, as on macOS AppleClang, the <filesystem> gives incorrect results on non-existing absolute paths,
// Which don't make sense anyway.

std::move(tos.begin(), tos.end(), std::back_inserter(tests));

for (const auto& [a, b, expected] : tests) {
    std::string result = fs_proximate_to(a, b);
    if (result != expected) {
        fail++;
        std::cerr << "FAIL: proximate_to(" << a << ", " << b << ") -> "  << result  << " (expected: "  << expected << ")\n";
    } else {
        std::cout << "PASS: proximate_to(" << a << ", " << b << ") -> "  << result  << "\n";
    }
}

if(fail){
    std::cerr << "FAIL: proximate " << fail << " tests failed.  backend: " << fs_backend() << "\n";
    return EXIT_FAILURE;
}

  ok_msg("proximate_to C++");

  return EXIT_SUCCESS;
}
