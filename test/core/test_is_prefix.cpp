#include "ffilesystem.h"
#include <string>
#include <tuple>

#include <gtest/gtest.h>


class TestPrefix : public ::testing::TestWithParam<std::tuple<std::string, std::string, bool>> {};

TEST_P(TestPrefix, IsPrefix) {
    auto [base, pre, expected] = GetParam();
    EXPECT_EQ(fs_is_prefix(base, pre), expected);
}

INSTANTIATE_TEST_SUITE_P(
    PrefixTests,
    TestPrefix,
    ::testing::Values(
        std::make_tuple("a/b//c", "a/b", false),
        std::make_tuple("a/b/c", "a/b/", false),
        std::make_tuple("a/b/c", "a", false),
        std::make_tuple("a/b", "a/b", true),
        std::make_tuple("a/b/", "a/b", true),
        std::make_tuple("/a/b", "a/b", false),
        std::make_tuple("a/b", "/a/b", false),
        std::make_tuple("a/b", "a/b/", true),
        std::make_tuple("a/b", "c", false),
        std::make_tuple("b", "a/b", false),
        std::make_tuple("c:/a", "c:/", false),
        std::make_tuple("c:/", "c:/a", true)
    )
);
