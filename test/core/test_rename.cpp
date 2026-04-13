#include <string>

#include "ffilesystem.h"

#include <gtest/gtest.h>

class TestRename: public testing::Test {
  protected:
    std::string f1, f2, in_file;
    std::string_view nonnull_file;

    void SetUp() override {
      f1 = "test_Ffs_rename.txt";
      f2 = "test_Ffs_rename2.txt";

      if(!fs_is_writable(".")){
        GTEST_SKIP() << "current directory is not writable";
      }

      ASSERT_TRUE(fs_touch(f1));
      ASSERT_TRUE(fs_is_file(f1));
      if(fs_exists(f2)){
        ASSERT_TRUE(fs_remove(f2));
      }

      in_file = f1 + "-invalid-memory-trailing-non-null-terminated-string_view";
      nonnull_file = std::string_view(in_file.data(), f1.size());
      ASSERT_NE(nonnull_file.back(), '\0') << "nonnull_file should not be null-terminated\n";
    }

    void TearDown() override {
      fs_remove(f2);
      fs_remove(in_file);
    }
  };


TEST_F(TestRename, Rename){

EXPECT_TRUE(fs_rename(f1, f2));
EXPECT_TRUE(fs_is_file(f2));

ASSERT_TRUE(fs_rename(f2, nonnull_file));
EXPECT_FALSE(fs_is_file(in_file)) << in_file << " should have been renamed to " << f1;
EXPECT_TRUE(fs_is_file(f1));
}
