#include "ffilesystem.h"
#include <iostream>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

class TestPermissions : public testing::Test {

  protected:
  std::string read, noread, nowrite, in_file;
  std::string_view nonnull_file;

    void SetUp() override {
      auto inst = testing::UnitTest::GetInstance();
      auto info = inst->current_test_info();
      std::string test_name_ = info->name();
      std::string test_suite_name_ = info->test_suite_name();
      std::string n = test_suite_name_ + "-" + test_name_;

      read = n + "readable.txt";
      noread = n + "nonreadable.txt";
      nowrite = n + "nonwritable.txt";

      if(!fs_is_writable(".")){
        GTEST_SKIP() << "current directory is not writable";
      }

      ASSERT_TRUE(fs_touch(read));
      ASSERT_TRUE(fs_is_file(read));

      ASSERT_TRUE(fs_touch(noread));
      ASSERT_TRUE(fs_is_file(noread));
      if(!fs_is_file(nowrite)){
        ASSERT_TRUE(fs_touch(nowrite));
      }

      ASSERT_TRUE(fs_exists(nowrite));
      ASSERT_TRUE(fs_is_file(nowrite));

      in_file = read + "-read_past_the_end_of_buffer";
      nonnull_file = std::string_view(in_file.data(), read.size());
      ASSERT_NE(nonnull_file.back(), '\0');
    }
    void TearDown() override {
      fs_remove(read);
      fs_remove(noread);
      fs_remove(nowrite);
    }
  };

TEST_F(TestPermissions, Empty){
EXPECT_TRUE(fs_get_permissions("").empty());
EXPECT_TRUE(fs_get_permissions("nonexistent.txt").empty());
EXPECT_FALSE(fs_get_permissions(read).empty());
}

TEST_F(TestPermissions, IsReadable){
EXPECT_TRUE(fs_is_readable(read)) << read << " should be readable";
}

TEST_F(TestPermissions, NotReadable){
// for Ffilesystem, even non-readable files "exist" and are "is_file"
ASSERT_TRUE(fs_set_permissions(noread, -1, 0, 0));
const std::string p = fs_get_permissions(noread);

std::cout << "Permissions: " << noread << " " << p << "\n";

if(!(fs_is_windows() || fs_is_cygwin())){
  EXPECT_EQ(p[0], '-');
}
}

TEST_F(TestPermissions, Read){
EXPECT_TRUE(fs_set_permissions(read, 1, 0, 0));
EXPECT_TRUE(fs_is_readable(read));
EXPECT_TRUE(fs_set_permissions(nonnull_file, 1, 0, 0));
}

TEST_F(TestPermissions, Writable)
{

// writable
ASSERT_TRUE(fs_set_permissions(nowrite, 0, -1, 0));

const std::string p = fs_get_permissions(nowrite);

// MSVC with <filesystem>, but we'll skip all windows
if (!fs_is_windows()){
  EXPECT_EQ(p[1], '-');

  if(!fs_is_admin()){
    EXPECT_FALSE(fs_is_writable(nowrite));
  }
}

}
