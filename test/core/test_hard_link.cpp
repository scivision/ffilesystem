#include "ffilesystem.h"

#include <iostream>
#include <cstdint>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

TEST(TestHardLink, HardLink)
{
EXPECT_GE(fs_hard_link_count("."), 1);

constexpr std::uintmax_t e = static_cast<std::uintmax_t>(-1);
EXPECT_EQ(fs_hard_link_count("not-exist-file"), e) << "backend " << fs_backend();

std::cout << "the return code for errors e.g. not existing file is " << e << "\n";
}


TEST(TestDevice, BlkSize)
{
  auto b1 = fs_get_blksize(testing::TempDir());
  EXPECT_GT(b1, 0);

  if(!fs_is_windows()) {
    std::string in2 = "./invalid-memory-trailing-non-null-terminated-string_view";
    std::string_view v(in2.data(), 2);
    ASSERT_NE(v.back(), '\0') << "v should not be null-terminated\n";
    EXPECT_EQ(fs_get_blksize(v), b1) << "fs_get_blksize(" << v << ") should be the same as fs_get_blksize(" << testing::TempDir() << ")\n";
  }
}



TEST(TestDevice, Device)
{
  std::string_view in = "./";
  EXPECT_GT(fs_st_dev(in), 0);

  std::string in2 = "./invalid-memory-trailing-non-null-terminated-string_view";
  std::string_view v(in2.data(), 2);
  ASSERT_NE(v.back(), '\0') << "v should not be null-terminated\n";
  EXPECT_EQ(fs_st_dev(v), fs_st_dev(in)) << "fs_st_dev(" << v << ") should be the same as fs_st_dev(" << in << ")\n";

  EXPECT_EQ(fs_st_dev("not-exist-file"), static_cast<dev_t>(-1)) << "backend " << fs_backend();
}


TEST(TestDevice, Inode)
{
  if (fs_is_windows())
#if !defined(HAVE_GETFILEINFORMATIONBYNAME)
    GTEST_SKIP() << "GetFileInformationByName is not available\n";
#endif

  std::string_view in = "./";
  const auto inode_dot = fs_inode(in);
  ASSERT_GT(inode_dot, 0);

  std::string in2 = "./invalid-memory-trailing-non-null-terminated-string_view";
  std::string_view v(in2.data(), 2);
  ASSERT_NE(v.back(), '\0') << "v should not be null-terminated\n";
  EXPECT_EQ(fs_inode(v), inode_dot) << "fs_inode(" << v << ") should be the same as fs_inode(" << in << ")\n";

  EXPECT_EQ(fs_inode("not-exist-file"), static_cast<ino_t>(0)) << "backend " << fs_backend();
}
