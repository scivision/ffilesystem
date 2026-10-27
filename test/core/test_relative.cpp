#include "ffilesystem.h"

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <boost/ut.hpp>

int main() {
  using namespace boost::ut;

  auto expect_relative = [](auto const& test_case) {
    expect(eq(fs_relative_to(test_case.first.first, test_case.first.second),
              test_case.second));
  };

  "relative_to"_test = [expect_relative](auto const& test_case) {
    expect_relative(test_case);
  } | std::vector<std::pair<std::pair<std::string_view, std::string_view>,
                            std::string_view>>{
          {{"", ""}, ""},
          {{"Hello", "Hello"}, "."},
          {{"Hello", "Hello/"}, "."},
          {{"a/b", "a/b"}, "."},
          {{"a///b", "a/b"}, "."},
          {{"a/b", "a///b"}, "."},
          {{"a/b", "a/b/"}, "."},
          {{"a/b/", "a/b"}, "."},
          {{"a/b", "a"}, ".."},
          {{"a/b", "a/"}, "../"},
          {{"a", "a/b/"}, "b/"},
          {{"a", "a/b/."}, "b/"},
          {{"a", "a/b/.."}, "."},
          {{"a/b/c/d", "a/b"}, "../.."},
          {{"a/b/c/d", "a/b/"}, "../../"},
          {{"./a/b", "./a/c"}, "../c"},
          {{"", "a"}, "a"},
          {{"/", "/"}, "."},
          {{"/dev/null", "/dev/null"}, "."},
          {{"a/b", "c/d"}, "../../c/d"},
          {{"c", "a/b"}, "../a/b"},
          {{"a/b", "a/c"}, "../c"},
      };

  if (ffilesystem::is_windows) {
    auto drive = fs_getenv("SYSTEMDRIVE");
    expect(drive.has_value() >> fatal) << "Failed to get SYSTEMDRIVE environment variable";
    const std::string c = drive.value();

    // NOTE: on Windows, if a path is real, finalPath is used, which makes drive letters upper case.
    const std::vector<std::pair<std::pair<std::string, std::string>, std::string>>
        windows_cases{
            {{c + "/", c + "/a/b"}, "a/b"},
            {{c + "/a/b", c + "/"}, "../.."},
            {{c + "/a/b", c + "/a/b"}, "."},
            {{c + "/a/b", c + "/a"}, ".."},
        };
    for (const auto& test_case : windows_cases) {
      expect_relative(test_case);
    }
    // {c+"/a", "b", ""} is ambiguous with Clang/Flang ARM MinGW <filesystem>.
  }

  // NOTE: use relative non-existing paths, as on macOS AppleClang, the <filesystem>
  // gives incorrect results on non-existing absolute paths, which don't make sense anyway.
}
