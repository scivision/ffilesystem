#include "ffilesystem.h"

#include <iostream>
#include <cstdint>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

class TestDevice : public testing::Test {
  protected:
    std::string in2;
    std::string_view nonnull2;

    void SetUp() override {
      in2 = "./invalid-memory-trailing-non-null-terminated-string_view";
      nonnull2 = std::string_view(in2.data(), 2);
      ASSERT_NE(nonnull2.back(), '\0') << "nonnull2 should not be null-terminated\n";
    }
  };

TEST_F(TestDevice, HardLink)
{
EXPECT_GE(fs_hard_link_count("."), 1);

EXPECT_EQ(fs_hard_link_count("not-exist-file"), fs_unknown_size) << "backend " << fs_backend();

std::cout << "the return code for errors e.g. not existing file is " << fs_unknown_size << "\n";

ASSERT_NE(fs_hard_link_count(nonnull2), fs_unknown_size) << "fs_hard_link_count(" << nonnull2 << ") should not return error code " << fs_unknown_size;
EXPECT_GE(fs_hard_link_count(nonnull2), 1);
}


TEST_F(TestDevice, BlkSize)
{
  auto b1 = fs_get_blksize(testing::TempDir());
  EXPECT_GT(b1, 0);

  if(fs_is_windows())
    return;

  b1 = fs_get_blksize(".");
  EXPECT_GT(b1, 0);
  EXPECT_EQ(fs_get_blksize(nonnull2), b1) << "fs_get_blksize(" << nonnull2 << ") should be the same as fs_get_blksize(" << testing::TempDir() << ")";
}



TEST_F(TestDevice, Device)
{
  std::string_view in = "./";
  EXPECT_GT(fs_st_dev(in), 0);

  EXPECT_EQ(fs_st_dev(nonnull2), fs_st_dev(in)) << "fs_st_dev(" << nonnull2 << ") should be the same as fs_st_dev(" << in << ")";

  EXPECT_EQ(fs_st_dev("not-exist-file"), static_cast<dev_t>(-1)) << "backend " << fs_backend();
}


TEST_F(TestDevice, Inode)
{
  if (fs_is_windows())
#if !defined(HAVE_GETFILEINFORMATIONBYNAME)
    GTEST_SKIP() << "GetFileInformationByName is not available\n";
#endif

  std::string_view in = "./";
  const auto inode_dot = fs_inode(in);
  ASSERT_GT(inode_dot, 0);

  EXPECT_EQ(fs_inode(nonnull2), inode_dot) << "fs_inode(" << nonnull2 << ") should be the same as fs_inode(" << in << ")";

  EXPECT_EQ(fs_inode("not-exist-file"), static_cast<ino_t>(0)) << "backend " << fs_backend();
}
