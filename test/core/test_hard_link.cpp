#include "ffilesystem.h"

#include <iostream>
#include <cstdint>

#include <gtest/gtest.h>

TEST(TestHardLink, HardLink)
{
EXPECT_GE(fs_hard_link_count("."), 1);

constexpr std::uintmax_t e = static_cast<std::uintmax_t>(-1);
EXPECT_EQ(fs_hard_link_count("not-exist-file"), e) << "backend " << fs_backend();

std::cout << "the return code for errors e.g. not existing file is " << e << "\n";
}
