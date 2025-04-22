#include "ffilesystem.h"
#include <iostream>

#include <gtest/gtest.h>

class TestPermissions : public testing::Test {

  protected:
  std::string read = "readable.txt";
  std::string noread = "nonreadable.txt";
  std::string nowrite = "nonwritable.txt";
    void SetUp() override {
      ASSERT_TRUE(fs_touch(read));
      ASSERT_TRUE(fs_exists(read));
      ASSERT_TRUE(fs_is_file(read));
      ASSERT_TRUE(fs_touch(noread));
      ASSERT_TRUE(fs_exists(noread));
      ASSERT_TRUE(fs_is_file(noread));
      if(!fs_is_file(nowrite)){
        ASSERT_TRUE(fs_touch(nowrite));
      }

      ASSERT_TRUE(fs_exists(nowrite));
      ASSERT_TRUE(fs_is_file(nowrite));
    }
    void TearDown() override {
      std::remove(read.c_str());
      std::remove(noread.c_str());
      std::remove(nowrite.c_str());
    }
  };

TEST_F(TestPermissions, Permissions)
{

EXPECT_TRUE(fs_get_permissions("").empty());
EXPECT_TRUE(fs_get_permissions("nonexistent.txt").empty());

EXPECT_TRUE(fs_set_permissions(read, 1, 0, 0));
EXPECT_FALSE(fs_get_permissions(read).empty());
EXPECT_TRUE(fs_is_readable(read));

// for Ffilesystem, even non-readable files "exist" and are "is_file"
EXPECT_TRUE(fs_set_permissions(noread, -1, 0, 0));

std::string p = fs_get_permissions(noread);

std::cout << "Permissions: " << noread << " " << p << "\n";

if(!(fs_is_windows() || fs_is_cygwin())){
  EXPECT_EQ(p[0], '-');
}

// writable
EXPECT_TRUE(fs_set_permissions(nowrite, 0, -1, 0));

p = fs_get_permissions(nowrite);

// MSVC with <filesystem>, but we'll skip all windows
if (!fs_is_windows()){
  EXPECT_EQ(p[1], '-');

  if(!fs_is_admin()){
    EXPECT_FALSE(fs_is_writable(nowrite));
  }
}

}
