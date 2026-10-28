#include "ffilesystem.h"

#include <string_view>
#include <utility>
#include <vector>

#include <boost/ut.hpp>

int main() {
  using namespace boost::ut;

  "is_subdir"_test = [](auto const& test_case) {
    expect(eq(ffilesystem::is_subdir(test_case.first.first, test_case.first.second),
              test_case.second));
  } | std::vector<std::pair<std::pair<std::string_view, std::string_view>, bool>>{
          {{"a/b/c", "a/b"}, true},
          {{"a/b/c", "a/b/"}, true},
          {{"a/b/c", "a"}, true},
          {{"a/b/", "a/b"}, false},
          {{"a/b", "a/b"}, false},
          {{"/a/b", "a/b"}, false},
          {{"a/b", "/a/b"}, false},
          {{"a/b", "a/b/"}, false},
          {{"a/b", "c"}, false},
          {{"b", "a/b"}, false},
          {{"c:/a", "c:/"}, true},
      };
}
