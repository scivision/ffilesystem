#include <string>
#include <tuple>

#include "ffilesystem.h"

#include <gtest/gtest.h>

class ParentTest : public ::testing::TestWithParam<std::tuple<std::string, std::string>> {};

TEST_P(ParentTest, Parent) {
  auto [inp, exp] = GetParam();

  if(fs_backend() == "<filesystem>"){
    if (fs_win32_is_ext_path(inp)) {
      GTEST_SKIP() << "<filesystem> doesn't yet support extended-length or device paths";
  }
}

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
  )
);

#if defined(_WIN32)
INSTANTIATE_TEST_SUITE_P(
  ParentWindows, ParentTest,
  ::testing::Values(
    std::make_tuple("c:\\a\\b/../.parent.txt", "c:/a/b/.."),
    std::make_tuple("x:/", "x:/"),
    std::make_tuple("x:\\", "x:/"),
    std::make_tuple(R"(\\?\C:\a\b/../.parent.txt)", R"(\\?\C:\a\b/..)")
  )
);
#endif
