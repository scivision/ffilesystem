#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestFilename, Agnostic)
{

EXPECT_EQ(fs_file_name(""), "");
EXPECT_EQ(fs_file_name("/"), "");
EXPECT_EQ(fs_file_name("."), ".");
EXPECT_EQ(fs_file_name("./"), "");
EXPECT_EQ(fs_file_name(".."), "..");
EXPECT_EQ(fs_file_name("../"), "");
EXPECT_EQ(fs_file_name("a"), "a");
EXPECT_EQ(fs_file_name("a/"), "");
EXPECT_EQ(fs_file_name("a/."), ".");
EXPECT_EQ(fs_file_name("a/.."), "..");
EXPECT_EQ(fs_file_name("a/b"), "b");
EXPECT_EQ(fs_file_name("a/b/"), "");
EXPECT_EQ(fs_file_name("a/b/c"), "c");
EXPECT_EQ(fs_file_name("a/b sdc/some space"), "some space");
EXPECT_EQ(fs_file_name("ab/.parent"), ".parent");
EXPECT_EQ(fs_file_name("ab/.parent.txt"), ".parent.txt");
EXPECT_EQ(fs_file_name("a/b/../.parent.txt"), ".parent.txt");
EXPECT_EQ(fs_file_name("/.fil"), ".fil");
EXPECT_EQ(fs_file_name("./日本語"), "日本語");
}

TEST(TestFilename, Windows)
{
if(!fs_is_windows())
  GTEST_SKIP() << "Windows only test";

EXPECT_EQ(fs_file_name("C:/"), "");
EXPECT_EQ(fs_file_name(R"(C:\ab\asb)"), "asb");

EXPECT_EQ(fs_file_name(R"(\\?\)"), "");
EXPECT_EQ(fs_file_name(R"(\\.\)"), "");

EXPECT_EQ(fs_file_name(R"(\\?\C:\)"), "");
EXPECT_EQ(fs_file_name(R"(\\.\C:\)"), "");
EXPECT_EQ(fs_file_name(R"(\\?\UNC\server\share)"), "share");
EXPECT_EQ(fs_file_name(R"(\\?\UNC\server\share\日本語)"), "日本語");
EXPECT_EQ(fs_file_name(R"(\\server\share\some space here.txt)"), "some space here.txt");
EXPECT_EQ(fs_file_name(R"(\\?\C:\some space here.txt)"), "some space here.txt");
}


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
