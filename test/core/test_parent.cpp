#include <string_view>
#include <vector>
#include <tuple>

#include "ffilesystem.h"

#include <gtest/gtest.h>


TEST(TestParent, Parent){

  std::vector<std::tuple<std::string_view, std::string_view>> test_cases;

test_cases = {
    {"", "."}, {"/", "/"}, {".", "."}, {"./", "."}, {"..", "."}, {"../", "."},
    {"a", "."}, {"a/", "."}, {"a/.", "a"}, {"a/..", "a"}, {"a/b", "a"}, {"a/b/", "a"}, {"a/b/c", "a/b"},
    {"ab/.parent", "ab"}, {"ab/.parent.txt", "ab"}, {"a/b/../.parent.txt", "a/b/.."}
};

  if(fs_is_windows()){
    test_cases.emplace_back("c:\\a\\b/../.parent.txt", "c:/a/b/..");
    test_cases.emplace_back("x:/", "x:/");
  }

  for (const auto& [input, expected] : test_cases)
    EXPECT_EQ(fs_parent(input), expected);

}
