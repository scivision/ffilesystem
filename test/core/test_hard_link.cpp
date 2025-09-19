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


TEST(TestDevice, Device)
{
EXPECT_GT(fs_st_dev("."), 0);
EXPECT_EQ(fs_st_dev("not-exist-file"), static_cast<dev_t>(-1)) << "backend " << fs_backend();
}


TEST(TestDevice, Inode)
{
if(!fs_is_windows()){
  EXPECT_GT(fs_inode("."), 0);
}

EXPECT_EQ(fs_inode("not-exist-file"), static_cast<ino_t>(0)) << "backend " << fs_backend();
}
