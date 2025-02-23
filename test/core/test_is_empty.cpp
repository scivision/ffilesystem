#include "ffilesystem.h"
#include <iostream>
#include <cstdio>  // std::remove

#include <gtest/gtest.h>

class TestEmpty : public testing::Test {
  protected:
    std::string dir;
    void SetUp() override {
      dir = "ffs_is_empty_empty_dir";
      ASSERT_TRUE(fs_mkdir(dir));
    }
    void TearDown() override {
      std::remove(dir.c_str());
    }
};

TEST_F(TestEmpty, IsEmpty)
{
    EXPECT_FALSE(fs_is_empty("."));
    EXPECT_TRUE(fs_is_empty(dir));

    EXPECT_FALSE(fs_is_empty(dir + "/not-exist-is-empty_cpp"));
}
