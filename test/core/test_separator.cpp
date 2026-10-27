#include "ffilesystem.h"

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <boost/ut.hpp>

int main() {
using namespace boost::ut;

"as_posix"_test = [](auto const& test_case) {
  std::string path{test_case.first};
  fs_as_posix(path);
  expect(eq(path, fs_is_windows() ? test_case.second : test_case.first));
} | std::vector<std::pair<std::string_view, std::string_view>>{
        {R"(a\b)", "a/b"},
        {R"(C:\my\path)", "C:/my/path"},
    };

"path_sep"_test = [] {
  expect(eq(ffilesystem::path_separator, (fs_is_windows()) ? ';' : ':'));
};

}
