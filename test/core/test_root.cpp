#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestRoot, Root){

EXPECT_EQ(fs_root(""), "");
EXPECT_EQ(fs_root("a/b"), "");
EXPECT_EQ(fs_root("./a/b"), "");
EXPECT_EQ(fs_root("../a/b"), "");


if (fs_is_windows()) {
  EXPECT_EQ(fs_root("c:"), "c:");
  EXPECT_EQ(fs_root("c:/a/b"), "c:/");
  EXPECT_EQ(fs_root("/etc"), "/");
  EXPECT_EQ(fs_root("\\etc"), "/");
  EXPECT_EQ(fs_root(R"(c:\)"), "c:/");
  EXPECT_EQ(fs_root("c:/"), "c:/");
  EXPECT_EQ(fs_root("\\"), "/");
} else {
  EXPECT_EQ(fs_root("/a/b"), "/");
  EXPECT_EQ(fs_root("c:/etc"), "");
}

}

TEST(TestRoot, RootName){
EXPECT_EQ(fs_root_name(""), "");
EXPECT_EQ(fs_root_name("a/b"), "");
EXPECT_EQ(fs_root_name("./a/b"), "");
EXPECT_EQ(fs_root_name("../a/b"), "");

if(fs_is_windows()){
  EXPECT_EQ(fs_root_name("c:/a/b"), "c:");
  EXPECT_EQ(fs_root_name("/etc"), "");
  EXPECT_EQ(fs_root_name(R"(C:\)"), "C:");
} else {
  EXPECT_EQ(fs_root_name("/a/b"), "");
  EXPECT_EQ(fs_root_name("c:/etc"), "");
}

}
