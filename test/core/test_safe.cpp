#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestSafe, Safe)
{

EXPECT_FALSE(fs_is_safe_name("test/re/"));
EXPECT_FALSE(fs_is_safe_name("test/re"));

if(fs_is_windows())
  EXPECT_FALSE(fs_is_safe_name("hi."));
else
  EXPECT_TRUE(fs_is_safe_name("hi."));

EXPECT_FALSE(fs_is_safe_name("hi there"));

EXPECT_FALSE(fs_is_safe_name("日本語"));

}
