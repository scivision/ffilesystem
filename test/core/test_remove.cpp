#include "ffilesystem.h"

#include <gtest/gtest.h>


class TestRemove : public testing::Test {
  protected:
    std::string file, in2;
    std::string_view nonnull2;
    void SetUp() override {
      file = "ffs_remove_test.txt";
      ASSERT_TRUE(fs_touch(file));

      in2 = "./" + file;
      nonnull2 = std::string_view(in2.data(), 2);
      ASSERT_NE(nonnull2.back(), '\0') << "nonnull2 should not be null-terminated\n";
    }
    void TearDown() override {
      fs_remove(file);
    }
  };


TEST_F(TestRemove, Remove)
{
ASSERT_FALSE(fs_remove(nonnull2)) << "Failed with input not null-terminated\n";

EXPECT_TRUE(fs_remove(file));
}
