#include <string>
#include <fstream>
#include <cstdint>

#include "ffilesystem.h"

#include <gtest/gtest.h>


class TestCopyFile : public testing::Test {
  protected:
    std::string s1, s2, s3, s4, t1;
    std::string ext1, ext5;
    std::uintmax_t iref;

    void SetUp() override {

      auto inst = testing::UnitTest::GetInstance();
      auto info = inst->current_test_info();
      std::string cwd = inst->original_working_dir();

      ASSERT_FALSE(cwd.empty());

      // https://google.github.io/googletest/reference/testing.html#UnitTest::current_test_suite
      std::string test_name_ = info->name();
      std::string test_suite_name_ = info->test_suite_name();
      std::string n = test_suite_name_ + "-" + test_name_;

      s1 = cwd + fs_filesep() + n + "_some_text.txt";
      s2 = cwd + fs_filesep() + n + "_some_text.txt.copy";
      s3 = cwd + fs_filesep() + n + "_empty.txt";
      s4 = cwd + fs_filesep() + n + "_empty.txt.copy";
      if(fs_is_windows()){
        ext1 = R"(\\?\)" + s1;
        ext5 = R"(\\?\)" + s2 + ".long";
      }

      t1 = "及せゃ市人購ゅトてへ投際ト点吉で速流つ今日";

      // Write to the first file
      std::ofstream ofs(s1);
      ASSERT_TRUE(ofs);
      ofs << t1;
      ofs.close(); // ensure flush

      iref = fs_file_size(s1);
      ASSERT_NE(iref, 0);

      std::cout << "TestCopyFile::SetUp: " << s1 << " bytes " << iref << "\n";

      ASSERT_TRUE(fs_touch(s3));
    }

    void TearDown() override {
      fs_remove(s1);
      fs_remove(s2);
      fs_remove(s3);
      fs_remove(s4);
      if(fs_is_windows()){
        fs_remove(ext5);
      }
    }
};

TEST_F(TestCopyFile, CopyFile){

std::string t2;

// Copy the file
EXPECT_TRUE(fs_copy_file(s1, s2, false));
EXPECT_TRUE(fs_is_file(s2));
EXPECT_FALSE(fs_copy_file(s1, s2, false)); // Should fail since s2 already exists

EXPECT_EQ(fs_file_size(s2), iref);

// Read from the copied file
std::ifstream ifs(s2);
std::getline(ifs, t2);

EXPECT_EQ(t1, t2);

EXPECT_TRUE(fs_copy_file(s3, s4, true));
EXPECT_TRUE(fs_is_file(s4));

EXPECT_EQ(fs_file_size(s4), 0);

}


TEST_F(TestCopyFile, Windows){
  if(!fs_is_windows())
    GTEST_SKIP() << "Windows specific test";


ASSERT_TRUE(fs_copy_file(ext1, ext5, false));
ASSERT_TRUE(fs_is_file(ext5));
EXPECT_EQ(fs_file_size(ext5), iref);


}
