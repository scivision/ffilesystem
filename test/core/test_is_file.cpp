#include "ffilesystem.h"

#include <cstdio>  // std::remove
#include <fstream>

#include <gtest/gtest.h>

class TestIsFile : public testing::Test {
protected:
  std::string file;
  void SetUp() override {
    file = testing::TempDir() + "/empty.txt";
    std::ofstream ofs(file);
    ofs.close();
  }
  void TearDown() override {
    std::remove(file.c_str());
  }
};


TEST_F(TestIsFile, IsFile)
{

  EXPECT_TRUE(fs_exists(file));
  EXPECT_TRUE(fs_is_file(file));
  EXPECT_FALSE(fs_is_dir(file));
  EXPECT_TRUE(fs_is_readable(file));
  EXPECT_FALSE(fs_is_exe(file));
  EXPECT_FALSE(fs_is_file(testing::TempDir() + "/not-exist-file"));
  EXPECT_FALSE(fs_is_file(""));
  EXPECT_FALSE(fs_is_file(testing::TempDir()));

}
