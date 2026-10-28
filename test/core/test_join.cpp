#include "ffilesystem.h"

#include <string_view>
#include <utility>
#include <vector>

#include <boost/ut.hpp>

int main() {
  using namespace boost::ut;

  "join"_test = [](auto const& test_case) {
    expect(eq(ffilesystem::join(test_case.first.first, test_case.first.second),
              test_case.second));
  } | std::vector<std::pair<std::pair<std::string_view, std::string_view>,
                            std::string_view>>{
          {{"", ""}, ""},
          {{"a", ""}, "a/"},
          {{"", "b"}, "b"},
          {{"a/b/", "c/"}, "a/b/c/"},
          {{"/", ""}, "/"},
          {{"", "/"}, "/"},
          {{"a", "b/"}, "a/b/"},
          {{"a/", "b/"}, "a/b/"},
          {{"a/b/../", "c/d/../"}, "a/b/../c/d/../"},
          {{"a/b", ".."}, "a/b/.."},
          {{"./a/b", "."}, "./a/b/."},
          {{"a/b", "c/d"}, "a/b/c/d"},
          {{"ab/cd", "/ef"}, "/ef"},
      };
}
