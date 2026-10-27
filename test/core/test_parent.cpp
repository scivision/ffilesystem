#include "ffilesystem.h"

#include <string_view>
#include <utility>
#include <vector>

#include <boost/ut.hpp>

int main() {
  using namespace boost::ut;

  "parent"_test = [](auto const& test_case) {
    expect(eq(fs_parent(test_case.first), test_case.second));
  } | std::vector<std::pair<std::string_view, std::string_view>>{
          {"", "."},
          {"/", "/"},
          {".", "."},
          {"./", "."},
          {"..", "."},
          {"../", "."},
          {"a", "."},
          {"a/", "."},
          {"a/.", "a"},
          {"a/..", "a"},
          {"a/b", "a"},
          {"a/b/", "a"},
          {"a/b/c", "a/b"},
          {"ab/.parent", "ab"},
          {"ab/.parent.txt", "ab"},
          {"a/b/../.parent.txt", "a/b/.."},
      };

  if (!ffilesystem::is_windows) {
    skip / "parent_windows"_test = [] {};
  } else {
    std::vector<std::pair<std::string_view, std::string_view>> windows_cases{
        {"c:\\a\\b/../.parent.txt", "c:\\a\\b/.."},
        {"x:/", "x:/"},
        {"x:\\", "x:/"},
    };
    if (ffilesystem::backend != "<filesystem>") {
      windows_cases.emplace_back(R"(\\?\C:\a\b/../.parent.txt)",
                                 R"(\\?\C:\a\b/..)");
    }

    "parent_windows"_test = [](auto const& test_case) {
      expect(eq(fs_parent(test_case.first), test_case.second));
    } | windows_cases;
  }

}
