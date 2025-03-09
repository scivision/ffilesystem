#include "ffilesystem.h"

#include <cstdio>  // std::remove

#include <gtest/gtest.h>

class TestOnDisk : public testing::Test {
  protected:
    std::string file, dir;
    void SetUp() override {
      file = "ffs_is_file_empty.txt";
      ASSERT_TRUE(fs_touch(file));
      ASSERT_TRUE(fs_exists(file));

      dir = "ffs_mkdir_test_dir";
      ASSERT_TRUE(fs_mkdir(dir));
      ASSERT_TRUE(fs_is_dir(dir));
    }
    void TearDown() override {
      std::remove(file.c_str());
      std::remove(dir.c_str());
    }
  };


TEST_F(TestOnDisk, IsDir)
{
  EXPECT_FALSE(fs_is_dir(""));
  EXPECT_TRUE(fs_is_dir("."));
  EXPECT_TRUE(fs_is_readable("."));
  EXPECT_FALSE(fs_is_dir(file));
  EXPECT_FALSE(fs_is_dir("ffs_is_dir_not-exist-dir"));
}


TEST_F(TestOnDisk, IsFile){
  EXPECT_TRUE(fs_is_file(file));
  EXPECT_TRUE(fs_is_readable(file));
  EXPECT_FALSE(fs_is_exe(file));
  EXPECT_FALSE(fs_is_file("ffs_is_file_not-exist-file"));
  EXPECT_FALSE(fs_is_file(""));
  EXPECT_FALSE(fs_is_file("."));
}


TEST_F(TestOnDisk, IsOther){
  EXPECT_FALSE(fs_is_other(""));
  EXPECT_FALSE(fs_is_other("."));
  EXPECT_FALSE(fs_is_other(file));
  EXPECT_FALSE(fs_is_other(dir));
  EXPECT_FALSE(fs_is_other("ffs_is_other_not-exist-file"));
}


TEST_F(TestOnDisk, Mkdir){

EXPECT_FALSE(fs_mkdir(""));

// Test mkdir with existing directory
EXPECT_TRUE(fs_mkdir(dir));

// Test mkdir with relative path
ASSERT_TRUE(fs_set_cwd(dir));

EXPECT_TRUE(fs_mkdir("test-filesystem-dir/hello"));
EXPECT_TRUE(fs_is_dir(fs_get_cwd() + "/test-filesystem-dir/hello"));
}
