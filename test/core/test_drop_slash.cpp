#include <iostream>
#include <cstdlib>
#include <vector>
#include <tuple>
#include <string_view>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main()
{
int fail = 0;

std::vector<std::tuple<std::string_view, std::string_view>> tests = {
  {"", ""},
  {"////", "/"},
  {"/", "/"},
  {"a////b", "a/b"},
  {"a//b//", "a/b"}
};

if (fs_is_windows()) {
  tests.push_back({"c:/", "c:/"});
  tests.push_back({"c:///", "c:/"});
  tests.push_back({"c:///", "c:/"});
  tests.push_back({"c:/a/b//", "c:/a/b"});
}


for (const auto& [a, b] : tests) {
  const std::string r = fs_drop_slash(a);
  if (r != b) {
    std::cerr << "FAIL: fs_drop_slash(" << a << ") != " << b << " got " << r << "\n";
    fail++;
  } else {
    std::cout << "PASS: fs_drop_slash(" << a << ") == " << b << "\n";
  }
}

if(fail){
  std::cerr << "FAILED: " << fail << " tests\n";
  return EXIT_FAILURE;
}

return EXIT_SUCCESS;

}
