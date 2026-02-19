#include "ffilesystem.h"
#include <string_view>

#include <gtest/gtest.h>

// /dev/stdin may not be available on CI systems

TEST(TestChar, IsChar)
{
  if (fs_is_windows()){
    EXPECT_TRUE(fs_is_char_device("NUL"));
    EXPECT_TRUE(fs_is_char_device("CONIN$"));
  } else {
    EXPECT_TRUE(fs_is_char_device("/dev/null"));
  }
}

TEST(TestChar, IsFile)
{
  if (fs_is_windows()){
    EXPECT_FALSE(fs_is_file("NUL"));
    EXPECT_FALSE(fs_is_file("CONIN$"));
  } else {
    EXPECT_FALSE(fs_is_file("/dev/null"));
  }
}

TEST(TestChar, Exists)
{
  if (fs_is_windows()){
    EXPECT_TRUE(fs_exists("NUL"));
    EXPECT_TRUE(fs_exists("CONIN$"));
  } else {
    EXPECT_TRUE(fs_exists("/dev/null"));
  }
}
