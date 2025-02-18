#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestHardLink, HardLink)
{
EXPECT_GE(fs_hard_link_count("."), 1);
}
