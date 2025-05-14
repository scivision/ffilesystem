#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestSeparator, AsPosix)
{

EXPECT_TRUE(fs_as_posix("").empty());

if(fs_is_windows()){
EXPECT_EQ(fs_as_posix(R"(a\b)"), "a/b");
EXPECT_EQ(fs_as_posix(R"(C:\my\path)"), "C:/my/path");
}

}

TEST(TestSeparator, PathSep) {

if(fs_is_windows()){
  EXPECT_EQ(fs_pathsep(), ';');
} else {
  EXPECT_EQ(fs_pathsep(), ':');
}
}
