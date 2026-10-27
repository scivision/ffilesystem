// verify functions handle empty input OK

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "ffilesystem.h"

#include <boost/ut.hpp>

int main() {
  using namespace boost::ut;

  "empty_input"_test = [] {
    constexpr std::string_view e = "";

    std::string s = "";
    fs_as_posix(s);
    expect(s.empty());

    "empty_input functions"_test = [](auto const& test_case) {
      expect(test_case.second("")) << test_case.first;
    } | std::vector<std::pair<std::string_view, bool (*)(std::string_view)>>{
            {"fs_file_name", [](std::string_view input) { return fs_file_name(input).empty(); }},
            {"fs_stem", [](std::string_view input) { return fs_stem(input).empty(); }},
            {"fs_join", [](std::string_view input) { return fs_join(input, input).empty(); }},
            {"fs_suffix", [](std::string_view input) { return fs_suffix(input).empty(); }},
            {"fs_with_suffix", [](std::string_view input) { return fs_with_suffix(input, input).empty(); }},
            {"fs_is_char_device", [](std::string_view input) { return !fs_is_char_device(input); }},
            {"fs_is_reserved", [](std::string_view input) { return !fs_is_reserved(input); }},
            {"fs_is_symlink", [](std::string_view input) { return !fs_is_symlink(input); }},
            {"fs_create_symlink", [](std::string_view input) { return !fs_create_symlink(input, input); }},
            {"fs_mkdir", [](std::string_view input) { return !fs_mkdir(input); }},
            {"fs_which", [](std::string_view input) { return fs_which(input).empty(); }},
            {"fs_root", [](std::string_view input) { return fs_root(input).empty(); }},
            {"fs_exists", [](std::string_view input) { return !fs_exists(input); }},
            {"fs_is_absolute", [](std::string_view input) { return !fs_is_absolute(input); }},
            {"fs_is_dir", [](std::string_view input) { return !fs_is_dir(input); }},
            {"fs_is_exe", [](std::string_view input) { return !fs_is_exe(input); }},
            {"fs_is_file", [](std::string_view input) { return !fs_is_file(input); }},
            {"fs_remove", [](std::string_view input) { return !fs_remove(input); }},
            {"fs_canonical", [](std::string_view input) { return fs_canonical(input, false).empty(); }},
            {"fs_equivalent", [](std::string_view input) { return !fs_equivalent(input, input); }},
            {"fs_expanduser", [](std::string_view input) { return fs_expanduser(input).empty(); }},
            {"fs_copy_file", [](std::string_view input) { return !fs_copy_file(input, input, false); }},
            {"fs_touch", [](std::string_view input) { return !fs_touch(input); }},
        };

    expect(eq(fs_file_size(e), fs_unknown_size)) << "backend: " << ffilesystem::backend;

    expect(!fs_get_cwd().empty());
    expect(!fs_get_homedir().empty());

    if (!ffilesystem::is_windows) {
      expect(eq(fs_space_available(e), fs_unknown_size)) << "backend: " << ffilesystem::backend;
      expect(!fs_set_permissions(e, 0, 0, 0));
    }
  };
}
