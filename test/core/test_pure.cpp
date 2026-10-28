#include "ffilesystem.h"

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <boost/ut.hpp>

int main() {
using namespace boost::ut;

"stem"_test = [](auto const& test_case) {
    expect(eq(ffilesystem::stem(test_case.first), std::string{test_case.second}));
} | std::vector<std::pair<std::string_view, std::string_view>>{
        {"", ""},
        {"stem.a.b", "stem.a"},
        {"stem.a", "stem"},
        {"stem", "stem"},
        {".stem", ".stem"},
        {"stem.", "stem"},
        {"stem.a.", "stem.a"},
        {"stem/a/b", "b"},
        {"./.stem", ".stem"},
        {"../.stem", ".stem"},
        {".stem.txt", ".stem"},
        {"a/..", ".."},
        {"a/../", ""},
        {"a/.", "."},
        {"日本語.日本語", "日本語"},
        {"some space.txt", "some space"},
        {"some space", "some space"},
    };

if (ffilesystem::is_windows) {
    "stem Windows"_test = [](auto const& test_case) {
        expect(eq(ffilesystem::stem(test_case.first), std::string{test_case.second}));
    } | std::vector<std::pair<std::string_view, std::string_view>>{
            {R"(C:\a\ball.text)", "ball"},
        };

    if (fs_win32_long_paths_enabled()) {
        "stem Windows long paths"_test = [](auto const& test_case) {
            expect(eq(ffilesystem::stem(test_case.first), std::string{test_case.second}));
        } | std::vector<std::pair<std::string_view, std::string_view>>{
                {R"(\\?\)", ""},
                {R"(\\.\)", ""},
                {R"(\\?\C:\)", ""},
                {R"(\\.\C:\)", ""},
                {R"(\\?\UNC\server\share)", "share"},
                {R"(\\?\UNC\server\share\日本語.txt)", "日本語"},
                {R"(\\server\share\some space here.txt)", "some space here"},
                {R"(\\?\C:\some space here.txt)", "some space here"},
            };
    }
}

"suffix"_test = [](auto const& test_case) {
    expect(eq(ffilesystem::suffix(test_case.first), std::string{test_case.second}));
} | std::vector<std::pair<std::string_view, std::string_view>>{
        {"", ""},
        {"a", ""},
        {"a.", "."},
        {"a.b", ".b"},
        {"a.b.c", ".c"},
        {"a/b.c", ".c"},
        {"a/b.c.d", ".d"},
        {"a/b/c.d", ".d"},
        {"a/b/c.d.e", ".e"},
        {"a/b/c.d/e", ""},
        {".a", ""},
        {".a.", "."},
        {".a.b", ".b"},
        {"./b.c", ".c"},
        {"../.b.c", ".c"},
        {"日本語.語", ".語"},
        {"some space.txt", ".txt"},
    };

if (ffilesystem::is_windows) {
    "suffix Windows"_test = [](auto const& test_case) {
        expect(eq(ffilesystem::suffix(test_case.first), std::string{test_case.second}));
    } | std::vector<std::pair<std::string_view, std::string_view>>{
            {R"(C:\a\ball.text)", ".text"},
        };

    if (fs_win32_long_paths_enabled()) {
        "suffix Windows long paths"_test = [](auto const& test_case) {
            expect(eq(ffilesystem::suffix(test_case.first), std::string{test_case.second}));
        } | std::vector<std::pair<std::string_view, std::string_view>>{
                {R"(\\?\)", ""},
                {R"(\\.\)", ""},
                {R"(\\?\C:\)", ""},
                {R"(\\.\C:\)", ""},
                {R"(\\?\UNC\server\share)", ""},
                {R"(\\?\UNC\server\share\日本語.txt)", ".txt"},
                {R"(\\server\share\some space here.txt)", ".txt"},
                {R"(\\?\C:\some space here.txt)", ".txt"},
            };
    }
}

}
