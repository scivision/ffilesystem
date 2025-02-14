#include "ffilesystem.h"

#include <gtest/gtest.h>


TEST(TestSuffix, Suffix)
{

EXPECT_EQ(fs_suffix(""), "");
EXPECT_EQ(fs_suffix("a"), "");
EXPECT_EQ(fs_suffix("a."), ".");
EXPECT_EQ(fs_suffix("a.b"), ".b");
EXPECT_EQ(fs_suffix("a.b.c"), ".c");
EXPECT_EQ(fs_suffix("a/b.c"), ".c");
EXPECT_EQ(fs_suffix("a/b.c.d"), ".d");
EXPECT_EQ(fs_suffix("a/b/c.d"), ".d");
EXPECT_EQ(fs_suffix("a/b/c.d.e"), ".e");
EXPECT_EQ(fs_suffix("a/b/c.d/e"), "");
EXPECT_EQ(fs_suffix(".a"), "");
EXPECT_EQ(fs_suffix(".a."), ".");
EXPECT_EQ(fs_suffix(".a.b"), ".b");
EXPECT_EQ(fs_suffix("./b.c"), ".c");
EXPECT_EQ(fs_suffix("../.b.c"), ".c");

}
