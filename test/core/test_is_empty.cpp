#include "ffilesystem.h"

#include <gtest/gtest.h>

class TestEmpty : public testing::Test {
  protected:
    std::string dir, in_dir;
    std::string_view nonnull_dir;

    void SetUp() override {
      if(!fs_is_writable(".")){
        GTEST_SKIP() << "current directory is not writable";
      }

      dir = "ffs_is_empty_empty_dir";
      ASSERT_TRUE(fs_mkdir(dir));

      in_dir = dir + "/read_past_the_end_of_buffer";
      nonnull_dir = std::string_view(in_dir.data(), dir.size());
      ASSERT_NE(nonnull_dir.back(), '\0');
    }
    void TearDown() override {
      fs_remove(dir);
    }
};

TEST_F(TestEmpty, IsEmpty)
{
    EXPECT_FALSE(fs_is_empty("."));
    EXPECT_TRUE(fs_is_empty(dir));

    EXPECT_FALSE(fs_is_empty(dir + "/not-exist-is-empty_cpp"));

    EXPECT_TRUE(fs_is_empty(nonnull_dir)) << "fs_is_empty() should not read past the end of string_view buffer";
}
