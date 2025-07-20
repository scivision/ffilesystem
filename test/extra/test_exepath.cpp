#include "ffilesystem.h"

#include <gtest/gtest.h>


TEST(TestExePath, ExePath){

  std::string path = fs_exe_path();

  ASSERT_FALSE(path.empty());
  ASSERT_TRUE(fs_exists(path));
  EXPECT_FALSE(fs_is_dir(path));
  EXPECT_TRUE(fs_is_file(path));
  EXPECT_TRUE(fs_is_exe(path));
  EXPECT_TRUE(fs_is_executable_binary(path));
  EXPECT_TRUE(fs_is_readable(path));
}
