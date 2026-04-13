#include "ffilesystem.h"
#include <gtest/gtest.h>


TEST(TestRoot, Root){

EXPECT_EQ(fs_root(""), "");
EXPECT_EQ(fs_root("a/b"), "");
EXPECT_EQ(fs_root("./a/b"), "");
EXPECT_EQ(fs_root("../a/b"), "");

if(fs_is_windows()){
EXPECT_EQ(fs_root("c:"), "c:");
EXPECT_EQ(fs_root("c:/a/b"), "c:/");
EXPECT_EQ(fs_root("/etc"), "/");
EXPECT_EQ(fs_root("\\etc"), "\\");
EXPECT_EQ(fs_root("c:\\"), "c:\\");
EXPECT_EQ(fs_root("c:/"), "c:/");
EXPECT_EQ(fs_root("\\"), "\\");
} else {
EXPECT_EQ(fs_root("/a/b"), "/");
EXPECT_EQ(fs_root("c:/etc"), "");
}

}


TEST(TestRootName, RootName){
EXPECT_EQ(fs_root_name(""), "");
EXPECT_EQ(fs_root_name("a/b"), "");
EXPECT_EQ(fs_root_name("./a/b"), "");
EXPECT_EQ(fs_root_name("../a/b"), "");

if (fs_is_windows()){
EXPECT_EQ(fs_root_name("c:/a/b"), "c:");
EXPECT_EQ(fs_root_name("/etc"), "");
EXPECT_EQ(fs_root_name(R"(C:\)"), "C:");

const std::string drive_prefixed = "C:/must-not-be-read";
const std::string_view truncated_drive(drive_prefixed.data(), 1);
ASSERT_EQ(truncated_drive, "C");
EXPECT_EQ(fs_root_name(truncated_drive), "")
  << "fs_root_name() read past string_view length for a non-null-terminated path";
} else {
EXPECT_EQ(fs_root_name("/a/b"), "");
EXPECT_EQ(fs_root_name("c:/etc"), "");
}

}
