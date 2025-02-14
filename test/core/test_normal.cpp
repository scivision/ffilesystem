#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestNormal, Normal)
{

EXPECT_EQ(fs_normal(""), ".");
EXPECT_EQ(fs_normal("/"), "/");
EXPECT_EQ(fs_normal("//"), "/");
EXPECT_EQ(fs_normal("/////"), "/");
EXPECT_EQ(fs_normal("."), ".");
EXPECT_EQ(fs_normal("./"), ".");
EXPECT_EQ(fs_normal("./."), ".");
EXPECT_EQ(fs_normal(".."), "..");
EXPECT_EQ(fs_normal("../"), "..");
EXPECT_EQ(fs_normal("a/.."), ".");
EXPECT_EQ(fs_normal("../.."), "../..");
EXPECT_EQ(fs_normal("a/b/.."), "a");
EXPECT_EQ(fs_normal("a/b/../.."), ".");
EXPECT_EQ(fs_normal("a/b/../../.."), "..");
EXPECT_EQ(fs_normal("/a"), "/a");
EXPECT_EQ(fs_normal("/a/"), "/a");
EXPECT_EQ(fs_normal("/a/."), "/a");
EXPECT_EQ(fs_normal("/a/.."), "/");
EXPECT_EQ(fs_normal("/a/b/.."), "/a");
EXPECT_EQ(fs_normal("a"), "a");
EXPECT_EQ(fs_normal(".a"), ".a");
EXPECT_EQ(fs_normal("a."), "a.");
EXPECT_EQ(fs_normal("a./"), "a.");
EXPECT_EQ(fs_normal("a/b"), "a/b");
EXPECT_EQ(fs_normal("..a"), "..a");
EXPECT_EQ(fs_normal("a.."), "a..");
EXPECT_EQ(fs_normal("a../"), "a..");
EXPECT_EQ(fs_normal("a/"), "a");
EXPECT_EQ(fs_normal("a//"), "a");
EXPECT_EQ(fs_normal("./a"), "a");
EXPECT_EQ(fs_normal("./a/"), "a");
EXPECT_EQ(fs_normal("./a/."), "a");
EXPECT_EQ(fs_normal("../a"), "../a");
EXPECT_EQ(fs_normal("../a/b/.."), "../a");
EXPECT_EQ(fs_normal("a/b/"), "a/b");
EXPECT_EQ(fs_normal("a/b/."), "a/b");
EXPECT_EQ(fs_normal("a/b/.."), "a");
EXPECT_EQ(fs_normal("a/b/../"), "a");
EXPECT_EQ(fs_normal("a/b/../c"), "a/c");
EXPECT_EQ(fs_normal("a/b/../c/d"), "a/c/d");
EXPECT_EQ(fs_normal("a/b/../../c/d"), "c/d");
EXPECT_EQ(fs_normal("././a/./b/././c/./."), "a/b/c");
EXPECT_EQ(fs_normal("a/b/../../c/../.."), "..");
EXPECT_EQ(fs_normal("a/b/../../../c/../.."), "../..");
EXPECT_EQ(fs_normal("a/./b/.."), "a");
EXPECT_EQ(fs_normal("a/.///b/../"), "a");
EXPECT_EQ(fs_normal("/a/../.."), "/");
EXPECT_EQ(fs_normal("/a/../../../../"), "/");

// some tests from https://github.com/gulrak/filesystem/blob/b1982f06c84f08a99fb90bac43c2d03712efe921/test/filesystem_test.cpp#L950

if(fs_is_windows()){
    EXPECT_EQ(fs_normal(R"(\/\///\/)"), "/");
    EXPECT_EQ(fs_normal(R"(a/b/..\//..///\/../c\\/)"), "../c");
    EXPECT_EQ(fs_normal(R"(..a/b/..\//..///\/../c\\/)"), "../c");
    EXPECT_EQ(fs_normal(R"(..\)"), "..");
    EXPECT_EQ(fs_normal(R"(c:\)"), "c:/");
    EXPECT_EQ(fs_normal(R"(c:\\)"), "c:/");
    EXPECT_EQ(fs_normal(R"(c:\a/b/../)"), "c:/a");
}

}
