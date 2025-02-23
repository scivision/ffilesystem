#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestSystem, System)
{
  EXPECT_FALSE(fs_compiler().empty()) << "unknown compiler";
  EXPECT_NE(fs_compiler().length(), fs_get_max_path()) << "compiler has blank space";

  EXPECT_FALSE(fs_get_username().empty());
  EXPECT_NE(fs_get_username().length(), fs_get_max_path()) << "username has blank space";
}
