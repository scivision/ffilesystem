#include "ffilesystem.h"

#include <gtest/gtest.h>


TEST(IsAbs,IsAbs)
{

EXPECT_FALSE(fs_is_absolute(""));

if(fs_is_windows()){
  EXPECT_TRUE(fs_is_absolute("J:/"));
  EXPECT_TRUE(fs_is_absolute("j:/"));
  EXPECT_FALSE(fs_is_absolute("j:"));
  EXPECT_FALSE(fs_is_absolute("/"));
  EXPECT_FALSE(fs_is_absolute("/日本語"));

} else {
  EXPECT_TRUE(fs_is_absolute("/"));
  EXPECT_TRUE(fs_is_absolute("/日本語"));
  EXPECT_FALSE(fs_is_absolute("j:/"));
}

  EXPECT_FALSE(fs_is_absolute("日本語"));
  EXPECT_FALSE(fs_is_absolute("some space here"));

}
