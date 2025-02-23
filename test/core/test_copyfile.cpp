#include <string>
#include <fstream>
#include <cstdio>  // std::remove

#include "ffilesystem.h"

#include <gtest/gtest.h>


class TestCopyFile : public testing::Test {
  protected:
    std::string s1, s2, s3, s4, t1;
    std::uintmax_t iref;

    void SetUp() override {
      s1 = "ffs_dummy_cpp.txt";
      s2 = "ffs_dummy_cpp.txt.copy";
      s3 = "ffs_empty_copyfile.txt";
      s4 = "ffs_empty_copyfile_copy.txt";
      t1 = "及せゃ市人購ゅトてへ投際ト点吉で速流つ今日";

      // Write to the first file
      std::ofstream ofs(s1);
      ASSERT_TRUE(ofs);
      ofs << t1;
      ofs.close();

      iref = fs_file_size(s1);
      ASSERT_NE(iref, 0);

      ASSERT_TRUE(fs_touch(s3));
    }
    void TearDown() override {
      std::remove(s1.c_str());
      std::remove(s2.c_str());
      std::remove(s3.c_str());
      std::remove(s4.c_str());
    }
};

TEST_F(TestCopyFile, CopyFile){

  std::string t2;

  // Copy the file
  EXPECT_TRUE(fs_copy_file(s1, s2, true));
  EXPECT_TRUE(fs_is_file(s2));

  EXPECT_EQ(fs_file_size(s2), iref);

  // Read from the copied file
  std::ifstream ifs(s2);
  std::getline(ifs, t2);
  ifs.close();

  EXPECT_EQ(t1, t2);

  EXPECT_TRUE(fs_copy_file(s3, s4, true));
  EXPECT_TRUE(fs_is_file(s4));

  EXPECT_EQ(fs_file_size(s4), 0);

}
