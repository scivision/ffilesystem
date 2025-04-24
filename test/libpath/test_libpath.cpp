#include "ffilesystem.h"

#include <gtest/gtest.h>


TEST(TestLibPath, LibPath){

  std::string path = fs_lib_path();

  ASSERT_FALSE(path.empty());
  ASSERT_TRUE(fs_exists(path));
  EXPECT_FALSE(fs_is_dir(path));
  EXPECT_TRUE(fs_is_file(path));
}
