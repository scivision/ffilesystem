#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <string_view>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main() {

int fail = 0;

std::vector<std::tuple<std::string_view, std::string_view>> tests = {
  {"", ""},
  {"a/b", ""},
  {"./a/b", ""},
  {"../a/b", ""}
};

if (fs_is_windows()) {
  tests.emplace_back("c:", "c:");
  tests.emplace_back("c:/a/b", "c:/");
  tests.emplace_back("/etc", "/");
  tests.emplace_back("\\etc", "/");
  tests.emplace_back(R"(c:\)", "c:/");
  tests.emplace_back("c:/", "c:/");
  tests.emplace_back("\\", "/");
} else {
  tests.emplace_back("/a/b", "/");
  tests.emplace_back("c:/etc", "");
}

for (const auto& [input, expected] : tests) {
  const std::string r = fs_root(input);
  if (r != expected) {
    std::cerr << "FAILED: root(" << input << ") = " << r << " != " << expected << "\n";
    fail++;
  }
}

// test root_name
tests = {
  {"", ""},
  {"a/b", ""},
  {"./a/b", ""},
  {"../a/b", ""}
};

if(fs_is_windows()){
  tests.emplace_back("c:/a/b", "c:");
  tests.emplace_back("/etc", "");
  tests.emplace_back(R"(C:\)", "C:");
} else {
  tests.emplace_back("/a/b", "");
  tests.emplace_back("c:/etc", "");
}

for(const auto& [input, expected] : tests){
  const std::string r = fs_root_name(input);
  if (r != expected) {
    std::cerr << "FAILED: root_name(" << input << ") = " << r << " != " << expected << "\n";
    fail++;
  }
}

if(fail){
  std::cerr << "ERROR: root(), root_name(): " << fail << " tests failed\n";
  return EXIT_FAILURE;
}

  ok_msg("root, root_name C++");

  return EXIT_SUCCESS;
}
