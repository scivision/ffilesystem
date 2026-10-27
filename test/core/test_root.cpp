#include "ffilesystem.h"

#include <string_view>
#include <utility>
#include <vector>

#include <boost/ut.hpp>

int main() {
  using namespace boost::ut;

  std::vector<std::pair<std::string_view, std::string_view>> root_cases{
      {"", ""},
      {"a/b", ""},
      {"./a/b", ""},
      {"../a/b", ""},
  };

  if (ffilesystem::is_windows) {
    root_cases.insert(root_cases.end(), {
        {"c:", "c:"},
        {"c:/a/b", "c:/"},
        {"/etc", "/"},
        {"\\etc", "\\"},
        {"c:\\", "c:\\"},
        {"c:/", "c:/"},
        {"\\", "\\"},
    });
  } else {
    root_cases.insert(root_cases.end(), {
        {"/a/b", "/"},
        {"c:/etc", ""},
    });
  }

  "root"_test = [](auto const& test_case) {
    expect(fs_root(test_case.first) == test_case.second);
  } | root_cases;

  std::vector<std::pair<std::string_view, std::string_view>> root_name_cases{
      {"", ""},
      {"a/b", ""},
      {"./a/b", ""},
      {"../a/b", ""},
  };

  if (ffilesystem::is_windows) {
    root_name_cases.insert(root_name_cases.end(), {
        {"c:/a/b", "c:"},
        {"/etc", ""},
        {R"(C:\)", "C:"},
    });
  } else {
    root_name_cases.insert(root_name_cases.end(), {
        {"/a/b", ""},
        {"c:/etc", ""},
    });
  }

  "root_name"_test = [](auto const& test_case) {
    expect(fs_root_name(test_case.first) == test_case.second);
    expect(fs_has_root_name(test_case.first) == !test_case.second.empty());
  } | root_name_cases;

  if (ffilesystem::is_windows) {
    constexpr char drive_prefixed[] = "C:/must-not-be-read";
    const std::string_view truncated_drive{drive_prefixed, 1};
    expect((truncated_drive == "C") >> fatal);
    expect(fs_root_name(truncated_drive) == "")
      << "fs_root_name() read past string_view length for a non-null-terminated path";
  }

  std::vector<std::pair<std::string_view, std::string_view>> root_directory_cases{
      {"", ""},
      {"a/b", ""},
      {"./a/b", ""},
      {"../a/b", ""},
  };

  if (ffilesystem::is_windows) {
    root_directory_cases.insert(root_directory_cases.end(), {
        {"c:", ""},
        {"c:/a/b", "/"},
        {"/etc", "/"},
        {"\\etc", "\\"},
        {"c:\\", "\\"},
        {"c:/", "/"},
        {"\\", "\\"},
    });
  } else {
    root_directory_cases.insert(root_directory_cases.end(), {
        {"/a/b", "/"},
        {"c:/etc", ""},
    });
  }

  "root_directory"_test = [](auto const& test_case) {
    expect(fs_root_directory(test_case.first) == test_case.second);
    expect(fs_has_root_directory(test_case.first) == !test_case.second.empty());
  } | root_directory_cases;
}
