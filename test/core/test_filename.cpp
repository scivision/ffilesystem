#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestFilename, Filename)
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
EXPECT_EQ(fs_file_name("ab/.parent"), ".parent");
EXPECT_EQ(fs_file_name("ab/.parent.txt"), ".parent.txt");
EXPECT_EQ(fs_file_name("a/b/../.parent.txt"), ".parent.txt");
EXPECT_EQ(fs_file_name("/.fil"), ".fil");
EXPECT_EQ(fs_file_name("./日本語"), "日本語");

if(fs_is_windows()){
  EXPECT_EQ(fs_file_name("C:/"), "");
  EXPECT_EQ(fs_file_name(R"(C:\ab\asb)"), "asb");
}


}
