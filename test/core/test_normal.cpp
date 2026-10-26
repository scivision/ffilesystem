#include "ffilesystem.h"
#include <string_view>
#include <utility>
#include <vector>

#include <boost/ut.hpp>

int main() {
using namespace boost::ut;

"normalize"_test = [](auto const& test_case) {
    expect(eq(fs_normal(test_case.first), test_case.second));
} | std::vector<std::pair<std::string_view, std::string_view>>{
{"", "."},
{"/", "/"},
{"//", "/"},
{"/////", "/"},
{".", "."},
{"./", "."},
{"./.", "."},
{"..", ".."},
{"../", ".."},
{"a/..", "."},
{"../..", "../.."},
{"a/b/..", "a"},
{"a/b/../..", "."},
{"a/b/../../..", ".."},
{"/a", "/a"},
{"/a/", "/a"},
{"/a/.", "/a"},
{"/a/..", "/"},
{"/a/b/..", "/a"},
{"a", "a"},
{".a", ".a"},
{"a.", "a."},
{"a./", "a."},
{"a/b", "a/b"},
{"..a", "..a"},
{"a..", "a.."},
{"a../", "a.."},
{"a/", "a"},
{"a//", "a"},
{"./a", "a"},
{"./a/", "a"},
{"./a/.", "a"},
{"../a", "../a"},
{"../a/b/..", "../a"},
{"a/b/", "a/b"},
{"a/b/.", "a/b"},
{"a/b/..", "a"},
{"a/b/../", "a"},
{"a/b/../c", "a/c"},
{"a/b/../c/d", "a/c/d"},
{"a/b/../../c/d", "c/d"},
{"././a/./b/././c/./.", "a/b/c"},
{"a/b/../../c/../..", ".."},
{"a/b/../../../c/../..", "../.."},
{"a/./b/..", "a"},
{"a/.///b/../", "a"},
{"/a/../..", "/"},
{"/a/../../../../", "/"},
{"./..", ".."},
{"../a/..", ".."},
{"../../a/..", "../.."},
{"a/../../b", "../b"},
{"/../", "/"},
{"/../../x", "/x"},
{"/a/../../b", "/b"},
{"a///b//c", "a/b/c"},
{"a//../b", "b"},
{"././.././a", "../a"},
{"...", "..."},
{"....//.", "...."},
{"a/.../..", "a"},
};

#if defined(_WIN32)
    "normalize_windows"_test = [](auto const& test_case) {
        expect(eq(fs_normal(test_case.first), test_case.second));
    } | std::vector<std::pair<std::string_view, std::string_view>>{
            {R"(\/\///\/)", "/"},
            {R"(a/b/..\//..///\/../c\\/)", "../c"},
            {R"(..a/b/..\//..///\/../c\\/)", "../c"},
            {R"(..\)", ".."},
            {"c:/", "c:/"},
            {"c://", "c:/"},
            {R"(c:\)", "c:/"},
            {R"(c:\\)", "c:/"},
            {R"(c:\a/b/../)", "c:/a"},
    };
#endif
// some tests from https://github.com/gulrak/filesystem/blob/b1982f06c84f08a99fb90bac43c2d03712efe921/test/filesystem_test.cpp#L950
}
