#include <string>
#include <cstdio> // for std::remove

#include "ffilesystem.h"

#include <gtest/gtest.h>

class TestRename: public testing::Test {
  protected:
    std::string f1, f2;
    void SetUp() override {
      f1 = "test_Ffs_rename.txt";
      f2 = "test_Ffs_rename2.txt";
      ASSERT_TRUE(fs_touch(f1));
      ASSERT_TRUE(fs_is_file(f1));
      if(fs_exists(f2))
        ASSERT_TRUE(fs_remove(f2));
    }
    void TearDown() override {
      std::remove(f2.c_str());
    }
  };


TEST_F(TestRename, Rename){

EXPECT_TRUE(fs_rename(f1, f2));
EXPECT_TRUE(fs_is_file(f2));
}
