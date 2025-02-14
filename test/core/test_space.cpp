#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestSpace, Space)
{

EXPECT_GT(fs_space_available(testing::TempDir()), 0);
EXPECT_GT(fs_space_capacity(testing::TempDir()), 0);

}
