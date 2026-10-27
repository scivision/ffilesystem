#include "ffilesystem.h"
#include <string>
#include <utility>
#include <vector>

#include <boost/ut.hpp>

int main() {
using namespace boost::ut;

"HasFilename"_test = [](auto const& test_case) {
    expect(fs_has_filename(test_case.first) == test_case.second);
} | std::vector<std::pair<std::string, bool>>{
        {"", false},
        {"/", false},
        {".", true},
        {"./", false},
        {"..", true},
        {"../", false},
        {"a", true},
        {"a/", false},
        {"a/.", true},
        {"a/..", true},
        {"a/b", true},
        {"a/b/", false},
        {"a/b/c", true},
        {"a/b sdc/some space", true},
        {"ab/.parent", true},
        {"ab/.parent.txt", true},
        {"a/b/../.parent.txt", true},
        {"/.fil", true},
        {"./日本語", true},
    };

if (ffilesystem::is_windows){
    "HasFilename Windows"_test = [](auto const& test_case) {
        expect(fs_has_filename(test_case.first) == test_case.second);
    } | std::vector<std::pair<std::string, bool>>{
            {"C:/", false},
            {R"(C:\ab\asb)", true},
            {R"(\\server\share\some space here.txt)", true},
        };

    if(fs_win32_long_paths_enabled()) {
        "HasFilename Windows long paths"_test = [](auto const& test_case) {
            expect(fs_has_filename(test_case.first) == test_case.second);
        } | std::vector<std::pair<std::string, bool>>{
                {R"(\\.\)", false},
                {R"(\\?\C:\)", false},
                {R"(\\.\C:\)", false},
                {R"(\\?\UNC\server\share)", true},
                {R"(\\?\UNC\server\share\日本語)", true},
                {R"(\\?\C:\some space here.txt)", true},
            };
    }
}

"FileName"_test = [](auto const& test_case) {
    expect(fs_file_name(test_case.first) == test_case.second);
} | std::vector<std::pair<std::string, std::string>>{
        {"", ""},
        {"/", ""},
        {".", "."},
        {"./", ""},
        {"..", ".."},
        {"../", ""},
        {"a", "a"},
        {"a/", ""},
        {"a/.", "."},
        {"a/..", ".."},
        {"a/b", "b"},
        {"a/b/", ""},
        {"a/b/c", "c"},
        {"a/b sdc/some space", "some space"},
        {"ab/.parent", ".parent"},
        {"ab/.parent.txt", ".parent.txt"},
        {"a/b/../.parent.txt", ".parent.txt"},
        {"/.fil", ".fil"},
        {"./日本語", "日本語"},
    };

if (ffilesystem::is_windows){
    "FileName Windows"_test = [](auto const& test_case) {
        expect(fs_file_name(test_case.first) == test_case.second);
    } | std::vector<std::pair<std::string, std::string>>{
            {"C:/", ""},
            {R"(C:\ab\asb)", "asb"},
        };

    if(fs_win32_long_paths_enabled()){
        "FileName Windows long paths"_test = [](auto const& test_case) {
            expect(fs_file_name(test_case.first) == test_case.second);
        } | std::vector<std::pair<std::string, std::string>>{
                {R"(\\?\)", ""},
                {R"(\\.\)", ""},
                {R"(\\?\C:\)", ""},
                {R"(\\.\C:\)", ""},
                {R"(\\?\UNC\server\share)", "share"},
                {R"(\\?\UNC\server\share\日本語)", "日本語"},
                {R"(\\server\share\some space here.txt)", "some space here.txt"},
                {R"(\\?\C:\some space here.txt)", "some space here.txt"},
            };
    }
}

}
