#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestSubdir, IsSubdir)
{

EXPECT_EQ(fs_is_subdir("a/b/c", "a/b"), true);
EXPECT_EQ(fs_is_subdir("a/b/c", "a/b/"), true);
EXPECT_EQ(fs_is_subdir("a/b/c", "a"), true);
EXPECT_EQ(fs_is_subdir("a/b", "a/b"), false);
EXPECT_EQ(fs_is_subdir("a/b", "a/b/"), false);
EXPECT_EQ(fs_is_subdir("a/b", "c"), false);
EXPECT_EQ(fs_is_subdir("b", "a/b"), false);
EXPECT_EQ(fs_is_subdir("c:/a", "c:/"), true);

}
