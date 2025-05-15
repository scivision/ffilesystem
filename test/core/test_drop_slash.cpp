#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestDropSlash, DropSlash)
{
EXPECT_EQ(fs_drop_slash(""), "");
EXPECT_EQ(fs_drop_slash("a"), "a");
EXPECT_EQ(fs_drop_slash("a/"), "a");
EXPECT_EQ(fs_drop_slash("a/b"), "a/b");
EXPECT_EQ(fs_drop_slash("a/b/"), "a/b");
EXPECT_EQ(fs_drop_slash("////"), "/");
EXPECT_EQ(fs_drop_slash("a////b"), "a/b");
EXPECT_EQ(fs_drop_slash("a//b//"), "a/b");
EXPECT_EQ(fs_drop_slash("/"), "/");

if (fs_is_windows()) {
  EXPECT_EQ(fs_drop_slash("c:/"), "c:/");
  EXPECT_EQ(fs_drop_slash("c:///"), "c:/");
  EXPECT_EQ(fs_drop_slash("c:///"), "c:/");
  EXPECT_EQ(fs_drop_slash("c:/a/b//"), "c:/a/b");

  EXPECT_EQ(fs_drop_slash(R"(\\?\C:/)"), R"(\\?\C:/)");
}

}
