#include "ffilesystem.h"
#include <string>
#include <tuple>

#include <gtest/gtest.h>


class TestSubdir : public ::testing::TestWithParam<std::tuple<std::string, std::string, bool>> {};

TEST_P(TestSubdir, IsSubdir)
{
    auto [base, sub, expected] = GetParam();
    EXPECT_EQ(fs_is_subdir(base, sub), expected);
}

INSTANTIATE_TEST_SUITE_P(
    SubdirTests,
    TestSubdir,
    ::testing::Values(
        std::make_tuple("a/b/c", "a/b", true),
        std::make_tuple("a/b/c", "a/b/", true),
        std::make_tuple("a/b/c", "a", true),
        std::make_tuple("a/b/", "a/b", false),
        std::make_tuple("a/b", "a/b", false),
        std::make_tuple("/a/b", "a/b", false),
        std::make_tuple("a/b", "/a/b", false),
        std::make_tuple("a/b", "a/b/", false),
        std::make_tuple("a/b", "c", false),
        std::make_tuple("b", "a/b", false),
        std::make_tuple("c:/a", "c:/", true)
    )
);
