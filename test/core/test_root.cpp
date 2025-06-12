#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestRoot, Agnostic){

EXPECT_EQ(fs_root(""), "");
EXPECT_EQ(fs_root("a/b"), "");
EXPECT_EQ(fs_root("./a/b"), "");
EXPECT_EQ(fs_root("../a/b"), "");
}

TEST(TestRoot, Windows){

if (!fs_is_windows())
  GTEST_SKIP() << "Windows only test";

EXPECT_EQ(fs_root("c:"), "c:");
EXPECT_EQ(fs_root("c:/a/b"), "c:/");
EXPECT_EQ(fs_root("/etc"), "/");
EXPECT_EQ(fs_root("\\etc"), "\\");
EXPECT_EQ(fs_root("c:\\"), "c:\\");
EXPECT_EQ(fs_root("c:/"), "c:/");
EXPECT_EQ(fs_root("\\"), "\\");
}

TEST(TestRoot, Posix){
if(fs_is_windows())
  GTEST_SKIP() << "Posix only test";

EXPECT_EQ(fs_root("/a/b"), "/");
EXPECT_EQ(fs_root("c:/etc"), "");
}


TEST(TestRootName, Agnostic){
EXPECT_EQ(fs_root_name(""), "");
EXPECT_EQ(fs_root_name("a/b"), "");
EXPECT_EQ(fs_root_name("./a/b"), "");
EXPECT_EQ(fs_root_name("../a/b"), "");
}

TEST(TestRootName, Windows){
if(!fs_is_windows())
  GTEST_SKIP() << "Windows only test";

EXPECT_EQ(fs_root_name("c:/a/b"), "c:");
EXPECT_EQ(fs_root_name("/etc"), "");
EXPECT_EQ(fs_root_name(R"(C:\)"), "C:");
}

TEST(TestRootName, Posix)
{
if(fs_is_windows())
  GTEST_SKIP() << "Posix only test";

EXPECT_EQ(fs_root_name("/a/b"), "");
EXPECT_EQ(fs_root_name("c:/etc"), "");
}
