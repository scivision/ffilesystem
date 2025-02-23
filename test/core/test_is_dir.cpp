#include "ffilesystem.h"

#include <cstdio>  // std::remove

#include <gtest/gtest.h>

class TestIsDir : public testing::Test {
protected:
  std::string file;
  void SetUp() override {
    file = "ffs_is_dir_empty.txt";
    ASSERT_TRUE(fs_touch(file));
  }
  void TearDown() override {
    std::remove(file.c_str());
  }
};


TEST_F(TestIsDir, IsDir)
{
  EXPECT_FALSE(fs_is_dir(""));

  EXPECT_TRUE(fs_is_dir("."));

  EXPECT_FALSE(fs_is_file("."));

  EXPECT_TRUE(fs_is_readable("."));

  EXPECT_FALSE(fs_is_dir(file));

  EXPECT_FALSE(fs_is_dir("ffs_is_dir_not-exist-dir"));
}
