#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestWithSuffix, WithSuffix)
{
  EXPECT_EQ(fs_with_suffix("", ".h5"), ".h5");
  EXPECT_EQ(fs_with_suffix("foo.h5", ""), "foo");
  EXPECT_EQ(fs_with_suffix(".foo.h5", ".txt"), ".foo.txt");
  EXPECT_EQ(fs_with_suffix(".h5", ""), ".h5");
  EXPECT_EQ(fs_with_suffix(".h5", ".h5"), ".h5.h5");
  EXPECT_EQ(fs_with_suffix("a//b///c/", ".h5"), "a/b/c/.h5");
  EXPECT_EQ(fs_with_suffix("c:/a/hi.nc", ".h5"), "c:/a/hi.h5");
  EXPECT_EQ(fs_with_suffix("my/file.h5", ".hdf5"), "my/file.hdf5");
  EXPECT_EQ(fs_with_suffix("a/boo", ".h5"), "a/boo.h5");
  EXPECT_EQ(fs_with_suffix("boo", ".h5"), "boo.h5");
  EXPECT_EQ(fs_with_suffix("a/b/c.d/", ".hdf5"), "a/b/c.d/.hdf5");
  EXPECT_EQ(fs_with_suffix("dir.h5/", ".hdf5"), "dir.h5/.hdf5");
  EXPECT_EQ(fs_with_suffix("a/b/.h5", ".nc"), "a/b/.h5.nc");
}
