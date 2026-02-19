#include "ffilesystem.h"

#include <gtest/gtest.h>


TEST(TestStem, Agnostic)
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

EXPECT_EQ(fs_stem("日本語.日本語"), "日本語");
EXPECT_EQ(fs_stem("some space.txt"), "some space");
EXPECT_EQ(fs_stem("some space"), "some space");
}

TEST(TestStem, Windows)
{
if(!fs_is_windows())
  GTEST_SKIP() << "Windows only test";

EXPECT_EQ(fs_stem(R"(C:\a\ball.text)"), "ball");

EXPECT_EQ(fs_stem(R"(\\?\)"), "");
EXPECT_EQ(fs_stem(R"(\\.\)"), "");

EXPECT_EQ(fs_stem(R"(\\?\C:\)"), "");
EXPECT_EQ(fs_stem(R"(\\.\C:\)"), "");
EXPECT_EQ(fs_stem(R"(\\?\UNC\server\share)"), "share");
EXPECT_EQ(fs_stem(R"(\\?\UNC\server\share\日本語.txt)"), "日本語");

EXPECT_EQ(fs_stem(R"(\\server\share\some space here.txt)"), "some space here");
EXPECT_EQ(fs_stem(R"(\\?\C:\some space here.txt)"), "some space here");
}

TEST(TestSuffix, Agnostic)
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
EXPECT_EQ(fs_suffix("日本語.語"), ".語");
EXPECT_EQ(fs_suffix("some space.txt"), ".txt");
}

TEST(TestSuffix, Windows)
{
if(!fs_is_windows())
  GTEST_SKIP() << "Windows only test";

EXPECT_EQ(fs_suffix(R"(C:\a\ball.text)"), ".text");

EXPECT_EQ(fs_suffix(R"(\\?\)"), "");
EXPECT_EQ(fs_suffix(R"(\\.\)"), "");

EXPECT_EQ(fs_suffix(R"(\\?\C:\)"), "");
EXPECT_EQ(fs_suffix(R"(\\.\C:\)"), "");
EXPECT_EQ(fs_suffix(R"(\\?\UNC\server\share)"), "");
EXPECT_EQ(fs_suffix(R"(\\?\UNC\server\share\日本語.txt)"), ".txt");

EXPECT_EQ(fs_suffix(R"(\\server\share\some space here.txt)"), ".txt");
EXPECT_EQ(fs_suffix(R"(\\?\C:\some space here.txt)"), ".txt");
}


TEST(TestWindowsUNC, UNC)
{
EXPECT_TRUE(fs_win32_is_ext_path(R"(\\.\C:\)"));
EXPECT_TRUE(fs_win32_is_ext_path(R"(\\?\C:\)"));
}
