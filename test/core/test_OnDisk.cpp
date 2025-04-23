#include "ffilesystem.h"

#include <cstdio>  // std::remove

#include <gtest/gtest.h>

namespace {
class TestOnDisk : public testing::Test {
  protected:
    std::string file, dir, cwd;
    void SetUp() override {
      auto inst = testing::UnitTest::GetInstance();
      auto info = inst->current_test_info();

      // https://google.github.io/googletest/reference/testing.html#UnitTest::current_test_suite
      std::string test_name_ = info->name();
      std::string test_suite_name_ = info->test_suite_name();
      std::string n = test_suite_name_ + "-" + test_name_;

      cwd = fs_as_posix(::testing::UnitTest::GetInstance()->original_working_dir());

      file = cwd + "/ffs_" + n + ".txt";
      ASSERT_TRUE(fs_touch(file));
      ASSERT_TRUE(fs_exists(file));

      dir = cwd + "/ffs_" + n + "_dir";
      ASSERT_TRUE(fs_mkdir(dir));
      ASSERT_TRUE(fs_is_dir(dir));
    }
    void TearDown() override {
      std::remove(file.c_str());
      std::remove(dir.c_str());
    }
  };


TEST_F(TestOnDisk, IsDir)
{
  EXPECT_FALSE(fs_is_dir(""));
  EXPECT_TRUE(fs_is_dir("."));
  EXPECT_TRUE(fs_is_dir(cwd));
  EXPECT_TRUE(fs_is_dir(dir));
  EXPECT_TRUE(fs_is_readable("."));
  EXPECT_FALSE(fs_is_dir(file));
  EXPECT_FALSE(fs_is_dir("ffs_is_dir_not-exist-dir"));
}


TEST_F(TestOnDisk, IsFile){
  EXPECT_TRUE(fs_is_file(file));
  EXPECT_FALSE(fs_is_exe(file));
  EXPECT_FALSE(fs_is_file("ffs_is_file_not-exist-file"));
  EXPECT_FALSE(fs_is_file(""));
  EXPECT_FALSE(fs_is_file("."));
  EXPECT_FALSE(fs_is_file(dir));
  EXPECT_FALSE(fs_is_file(cwd));
}


TEST_F(TestOnDisk, IsReadable){
  EXPECT_TRUE(fs_is_readable(file));
  EXPECT_TRUE(fs_is_readable(dir));
}


TEST_F(TestOnDisk, IsOther){
  EXPECT_FALSE(fs_is_other(""));
  EXPECT_FALSE(fs_is_other("."));
  EXPECT_FALSE(fs_is_other(file));
  EXPECT_FALSE(fs_is_other(dir));
  EXPECT_FALSE(fs_is_other(cwd));
  EXPECT_FALSE(fs_is_other("ffs_is_other_not-exist-file"));
}


TEST_F(TestOnDisk, Mkdir){

EXPECT_FALSE(fs_mkdir(""));

// Test mkdir with existing directory
ASSERT_TRUE(fs_mkdir(dir));

// Test mkdir with relative path
ASSERT_TRUE(fs_set_cwd(dir));

ASSERT_TRUE(fs_mkdir("test-filesystem-dir/hello"));
EXPECT_TRUE(fs_is_dir(dir + "/test-filesystem-dir/hello"));
}


TEST_F(TestOnDisk, Realpath){

EXPECT_EQ(fs_realpath("."), cwd);
EXPECT_TRUE(fs_realpath("not-exist-realpath/b/c").empty());

EXPECT_LT(fs_realpath("..").length(), cwd.length());
}


TEST_F(TestOnDisk, Touch)
{

EXPECT_TRUE(fs_touch(file));

auto t0 = fs_get_modtime(file);
EXPECT_GT(t0, 0);

EXPECT_TRUE(fs_set_modtime(file));

EXPECT_GE(fs_get_modtime(file), t0);

EXPECT_FALSE(fs_set_modtime("not-exist-file"));

}
}
