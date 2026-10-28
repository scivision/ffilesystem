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
    ffilesystem::as_posix(s);
    expect(s.empty());

    "empty_input functions"_test = [](auto const& test_case) {
      expect(test_case.second("")) << test_case.first;
    } | std::vector<std::pair<std::string_view, bool (*)(std::string_view)>>{
            {"ffilesystem::file_name", [](std::string_view input) { return ffilesystem::file_name(input).empty(); }},
            {"ffilesystem::stem", [](std::string_view input) { return ffilesystem::stem(input).empty(); }},
            {"ffilesystem::join", [](std::string_view input) { return ffilesystem::join(input, input).empty(); }},
            {"ffilesystem::suffix", [](std::string_view input) { return ffilesystem::suffix(input).empty(); }},
            {"ffilesystem::with_suffix", [](std::string_view input) { return ffilesystem::with_suffix(input, input).empty(); }},
            {"ffilesystem::is_char_device", [](std::string_view input) { return !ffilesystem::is_char_device(input); }},
            {"ffilesystem::is_reserved", [](std::string_view input) { return !ffilesystem::is_reserved(input); }},
            {"ffilesystem::is_symlink", [](std::string_view input) { return !ffilesystem::is_symlink(input); }},
            {"ffilesystem::create_symlink", [](std::string_view input) { return !ffilesystem::create_symlink(input, input); }},
            {"ffilesystem::mkdir", [](std::string_view input) { return !ffilesystem::mkdir(input); }},
            {"ffilesystem::which", [](std::string_view input) { return ffilesystem::which(input).empty(); }},
            {"ffilesystem::root", [](std::string_view input) { return ffilesystem::root(input).empty(); }},
            {"ffilesystem::exists", [](std::string_view input) { return !ffilesystem::exists(input); }},
            {"ffilesystem::is_absolute", [](std::string_view input) { return !ffilesystem::is_absolute(input); }},
            {"ffilesystem::is_directory", [](std::string_view input) { return !ffilesystem::is_directory(input); }},
            {"ffilesystem::is_exe", [](std::string_view input) { return !ffilesystem::is_exe(input); }},
            {"ffilesystem::is_file", [](std::string_view input) { return !ffilesystem::is_file(input); }},
            {"fs_remove", [](std::string_view input) { return !ffilesystem::remove(input); }},
            {"ffilesystem::canonical", [](std::string_view input) { return ffilesystem::canonical(input, false).empty(); }},
            {"ffilesystem::equivalent", [](std::string_view input) { return !ffilesystem::equivalent(input, input); }},
            {"ffilesystem::expanduser", [](std::string_view input) { return ffilesystem::expanduser(input).empty(); }},
            {"ffilesystem::copy_file", [](std::string_view input) { return !ffilesystem::copy_file(input, input, false); }},
            {"ffilesystem::touch", [](std::string_view input) { return !ffilesystem::touch(input); }},
        };

    expect(eq(ffilesystem::file_size(e), fs_unknown_size)) << "backend: " << ffilesystem::backend;

    expect(!ffilesystem::get_cwd().empty());
    expect(!ffilesystem::get_homedir().empty());

    if (!ffilesystem::is_windows) {
      expect(eq(ffilesystem::space_available(e), fs_unknown_size)) << "backend: " << ffilesystem::backend;
      expect(!ffilesystem::set_permissions(e, 0, 0, 0));
    }
  };
}
