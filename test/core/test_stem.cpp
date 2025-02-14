#include "ffilesystem.h"

#include <gtest/gtest.h>


TEST(TestStem, Stem)
{

EXPECT_EQ(fs_stem(""), "");
EXPECT_EQ(fs_stem("stem.a.b"), "stem.a");
EXPECT_EQ(fs_stem("stem.a"), "stem");
EXPECT_EQ(fs_stem("stem"), "stem");
EXPECT_EQ(fs_stem(".stem"), ".stem");
EXPECT_EQ(fs_stem("stem."), "stem");
EXPECT_EQ(fs_stem("stem.a."), "stem.a");
EXPECT_EQ(fs_stem("stem/a/b"), "b");
EXPECT_EQ(fs_stem("./.stem"), ".stem");
EXPECT_EQ(fs_stem("../.stem"), ".stem");
EXPECT_EQ(fs_stem(".stem.txt"), ".stem");
EXPECT_EQ(fs_stem("a/.."), "..");
EXPECT_EQ(fs_stem("a/../"), "");
EXPECT_EQ(fs_stem("a/."), ".");

if (fs_is_windows()) {
EXPECT_EQ(fs_stem(R"(C:\a\ball.text)"), "ball");
}

EXPECT_EQ(fs_stem("日本語.日本語"), "日本語");
EXPECT_EQ(fs_stem("some space.txt"), "some space");
EXPECT_EQ(fs_stem("some space"), "some space");
}
