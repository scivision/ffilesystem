#include <string>
#include <tuple>

#include "ffilesystem.h"

#include <gtest/gtest.h>

class ParentTest : public ::testing::TestWithParam<std::tuple<std::string, std::string>> {};

TEST_P(ParentTest, Parent) {
  auto [inp, exp] = GetParam();

  EXPECT_EQ(fs_parent(inp), exp);
}

INSTANTIATE_TEST_SUITE_P(
  Parent, ParentTest,
  ::testing::Values(
      std::make_tuple("", "."),
      std::make_tuple("/", "/"),
      std::make_tuple(".", "."),
      std::make_tuple("./", "."),
      std::make_tuple("..", "."),
      std::make_tuple("../", "."),
      std::make_tuple("a", "."),
      std::make_tuple("a/", "."),
      std::make_tuple("a/.", "a"),
      std::make_tuple("a/..", "a"),
      std::make_tuple("a/b", "a"),
      std::make_tuple("a/b/", "a"),
      std::make_tuple("a/b/c", "a/b"),
      std::make_tuple("ab/.parent", "ab"),
      std::make_tuple("ab/.parent.txt", "ab"),
      std::make_tuple("a/b/../.parent.txt", "a/b/..")
#if defined(_WIN32)
,
      std::make_tuple("c:\\a\\b/../.parent.txt", "c:/a/b/.."),
      std::make_tuple("x:/", "x:/")
#endif
  )
);

        // test_cases.emplace_back("c:\\a\\b/../.parent.txt", "c:/a/b/..");
        // test_cases.emplace_back("x:/", "x:/");
