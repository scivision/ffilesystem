#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestHasFilename, Agnostic)
{

EXPECT_FALSE(fs_has_filename(""));
EXPECT_FALSE(fs_has_filename("/"));
EXPECT_TRUE(fs_has_filename("."));
EXPECT_FALSE(fs_has_filename("./"));
EXPECT_TRUE(fs_has_filename(".."));
EXPECT_FALSE(fs_has_filename("../"));
EXPECT_TRUE(fs_has_filename("a"));
EXPECT_FALSE(fs_has_filename("a/"));
EXPECT_TRUE(fs_has_filename("a/."));
EXPECT_TRUE(fs_has_filename("a/.."));
EXPECT_TRUE(fs_has_filename("a/b"));
EXPECT_FALSE(fs_has_filename("a/b/"));
EXPECT_TRUE(fs_has_filename("a/b/c"));
EXPECT_TRUE(fs_has_filename("a/b sdc/some space"));
EXPECT_TRUE(fs_has_filename("ab/.parent"));
EXPECT_TRUE(fs_has_filename("ab/.parent.txt"));
EXPECT_TRUE(fs_has_filename("a/b/../.parent.txt"));
EXPECT_TRUE(fs_has_filename("/.fil"));
EXPECT_TRUE(fs_has_filename("./日本語"));
}

TEST(TestHasFilename, Windows)
{
if(!fs_is_windows())
  GTEST_SKIP() << "Windows only test";

EXPECT_FALSE(fs_has_filename("C:/"));
EXPECT_TRUE(fs_has_filename(R"(C:\ab\asb)"));

EXPECT_FALSE(fs_has_filename(R"(\\?\)"));
EXPECT_FALSE(fs_has_filename(R"(\\.\)"));

EXPECT_FALSE(fs_has_filename(R"(\\?\C:\)"));
EXPECT_FALSE(fs_has_filename(R"(\\.\C:\)"));
EXPECT_TRUE(fs_has_filename(R"(\\?\UNC\server\share)"));
EXPECT_TRUE(fs_has_filename(R"(\\?\UNC\server\share\日本語)"));
EXPECT_TRUE(fs_has_filename(R"(\\server\share\some space here.txt)"));
EXPECT_TRUE(fs_has_filename(R"(\\?\C:\some space here.txt)"));
}


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