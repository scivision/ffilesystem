#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <tuple>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int check(std::string_view in, std::string_view ref){
  std::string r = fs_file_name(in);
  if (r != ref) {
    std::cerr << "FAIL: filename(" << in << ") " << r << "\n";
    return 1;
  }
  return 0;
}

int main() {

  std::vector<std::tuple<std::string_view, std::string_view>> test_cases = {
    {"", ""}, {"/", ""}, {".", "."}, {"./", ""}, {"..", ".."}, {"../", ""},
    {"a", "a"}, {"a/", ""}, {"a/.", "."}, {"a/..", ".."}, {"a/b", "b"}, {"a/b/", ""}, {"a/b/c", "c"},
    {"ab/.parent", ".parent"}, {"ab/.parent.txt", ".parent.txt"}, {"a/b/../.parent.txt", ".parent.txt"}, {"./fil", "fil"},
    {"./日本語", "日本語"}
  };

  if(fs_is_windows()){
    test_cases.emplace_back("C:/", "");
    test_cases.emplace_back(R"(C:\ab\asb)", "asb");
  }

  int i = 0;

  for (const auto& [input, expected] : test_cases) {
    i += check(input, expected);
  }

  if (i != 0) {
    std::cerr << "FAIL: file_name()\n";
    return EXIT_FAILURE;
  }

  ok_msg("file_name C++");

  return EXIT_SUCCESS;
}
