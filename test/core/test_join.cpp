#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestJoin, Join)
{
  EXPECT_EQ(fs_join("", ""), "");
  EXPECT_EQ(fs_join("a", ""), "a");
  EXPECT_EQ(fs_join("", "b"), "b");
  EXPECT_EQ(fs_join("a//b/", "c/"), "a/b/c");
  EXPECT_EQ(fs_join("/", ""), "/");
  EXPECT_EQ(fs_join("", "/"), "/");
  EXPECT_EQ(fs_join("a", "b//"), "a/b");
  EXPECT_EQ(fs_join("a//", "b//"), "a/b");
  EXPECT_EQ(fs_join("a/b/../", "c/d/../"), "a/b/../c/d/..");
  EXPECT_EQ(fs_join("a/b", ".."), "a/b/..");
  EXPECT_EQ(fs_join("./a/b", "."), "./a/b/.");
  EXPECT_EQ(fs_join("a/b", "c/d"), "a/b/c/d");
  EXPECT_EQ(fs_join("ab/cd", "/ef"), "/ef");
}
