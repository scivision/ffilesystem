#include "ffilesystem.h"

#include <gtest/gtest.h>

// for Windows need an invalid drive as a non-existing relative path just gives the space anyway.

TEST(TestSpace, SpaceAvailable)
{
EXPECT_GT(fs_space_available(testing::TempDir()), 0);
EXPECT_EQ(fs_space_available("cc:/not-exist-available"), static_cast<std::uintmax_t>(-1)) << "backend " << fs_backend();
}

TEST(TestSpace, SpaceCapacity)
{
EXPECT_GT(fs_space_capacity(testing::TempDir()), 0);
EXPECT_EQ(fs_space_capacity("cc:/not-exist-capacity"), static_cast<std::uintmax_t>(-1)) << "backend " << fs_backend();
}

TEST(TestBlksize, BlkSize)
{
EXPECT_GT(fs_get_blksize(testing::TempDir()), 0);
}
