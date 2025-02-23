#include <cstdio>  // std::remove

#include "ffilesystem.h"

#include <gtest/gtest.h>

class TestMkdir : public testing::Test {
  protected:
    std::string dir;
    void SetUp() override {
      dir = "ffs_mkdir_test_dir";
      ASSERT_TRUE(fs_mkdir(dir));
      ASSERT_TRUE(fs_is_dir(dir));
    }
    void TearDown() override {
      std::remove(dir.c_str());
    }
  };

TEST_F(TestMkdir, Mkdir)
{

EXPECT_FALSE(fs_mkdir(""));

// Test mkdir with existing directory
EXPECT_TRUE(fs_mkdir(dir));

// Test mkdir with relative path
ASSERT_TRUE(fs_set_cwd(dir));

EXPECT_TRUE(fs_mkdir("test-filesystem-dir/hello"));
EXPECT_TRUE(fs_is_dir(fs_get_cwd() + "/test-filesystem-dir/hello"));

}
