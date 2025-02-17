#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestPrefix, IsPrefix)
{

EXPECT_EQ(fs_is_prefix("a/b//c", "a/b"), false);
EXPECT_EQ(fs_is_prefix("a/b/c", "a/b/"), false);
EXPECT_EQ(fs_is_prefix("a/b/c", "a"), false);
EXPECT_EQ(fs_is_prefix("a/b", "a/b"), true);
EXPECT_EQ(fs_is_prefix("a/b", "a/b/"), true);
EXPECT_EQ(fs_is_prefix("a/b", "c"), false);
EXPECT_EQ(fs_is_prefix("b", "a/b"), false);
EXPECT_EQ(fs_is_prefix("c:/a", "c:/"), false);
EXPECT_EQ(fs_is_prefix("c:/", "c:/a"), true);

}
