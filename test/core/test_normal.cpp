#include "ffilesystem.h"
#include <string>
#include <tuple>

#include <gtest/gtest.h>

class TestNormal : public ::testing::TestWithParam<std::tuple<std::string, std::string>> {};

TEST_P(TestNormal, Normalize)
{
    auto [inp, expected] = GetParam();
    EXPECT_EQ(fs_normal(inp), expected);
}


INSTANTIATE_TEST_SUITE_P(
    Normalize,
    TestNormal,
    ::testing::Values(

std::make_tuple("", "."),
std::make_tuple("/", "/"),
std::make_tuple("//", "/"),
std::make_tuple("/////", "/"),
std::make_tuple(".", "."),
std::make_tuple("./", "."),
std::make_tuple("./.", "."),
std::make_tuple("..", ".."),
std::make_tuple("../", ".."),
std::make_tuple("a/..", "."),
std::make_tuple("../..", "../.."),
std::make_tuple("a/b/..", "a"),
std::make_tuple("a/b/../..", "."),
std::make_tuple("a/b/../../..", ".."),
std::make_tuple("/a", "/a"),
std::make_tuple("/a/", "/a"),
std::make_tuple("/a/.", "/a"),
std::make_tuple("/a/..", "/"),
std::make_tuple("/a/b/..", "/a"),
std::make_tuple("a", "a"),
std::make_tuple(".a", ".a"),
std::make_tuple("a.", "a."),
std::make_tuple("a./", "a."),
std::make_tuple("a/b", "a/b"),
std::make_tuple("..a", "..a"),
std::make_tuple("a..", "a.."),
std::make_tuple("a../", "a.."),
std::make_tuple("a/", "a"),
std::make_tuple("a//", "a"),
std::make_tuple("./a", "a"),
std::make_tuple("./a/", "a"),
std::make_tuple("./a/.", "a"),
std::make_tuple("../a", "../a"),
std::make_tuple("../a/b/..", "../a"),
std::make_tuple("a/b/", "a/b"),
std::make_tuple("a/b/.", "a/b"),
std::make_tuple("a/b/..", "a"),
std::make_tuple("a/b/../", "a"),
std::make_tuple("a/b/../c", "a/c"),
std::make_tuple("a/b/../c/d", "a/c/d"),
std::make_tuple("a/b/../../c/d", "c/d"),
std::make_tuple("././a/./b/././c/./.", "a/b/c"),
std::make_tuple("a/b/../../c/../..", ".."),
std::make_tuple("a/b/../../../c/../..", "../.."),
std::make_tuple("a/./b/..", "a"),
std::make_tuple("a/.///b/../", "a"),
std::make_tuple("/a/../..", "/"),
std::make_tuple("/a/../../../../", "/")
    )
);

#if defined(_WIN32)
INSTANTIATE_TEST_SUITE_P(
    NormalizeWindows,
    TestNormal,
    ::testing::Values(

    std::make_tuple(R"(\/\///\/)", "/"),
    std::make_tuple(R"(a/b/..\//..///\/../c\\/)", "../c"),
    std::make_tuple(R"(..a/b/..\//..///\/../c\\/)", "../c"),
    std::make_tuple(R"(..\)", ".."),
    std::make_tuple(R"(c:\)", "c:/"),
    std::make_tuple(R"(c:\\)", "c:/"),
    std::make_tuple(R"(c:\a/b/../)", "c:/a")
    )
);
#endif
// some tests from https://github.com/gulrak/filesystem/blob/b1982f06c84f08a99fb90bac43c2d03712efe921/test/filesystem_test.cpp#L950
