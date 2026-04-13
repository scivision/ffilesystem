#include <gtest/gtest.h>

#include "ffilesystem.h"

class TestMkdir : public testing::Test {
  protected:
    std::string dir, cwd, in_dir;
    std::string_view nonnull_dir;

    void SetUp() override {
      auto inst = testing::UnitTest::GetInstance();
      auto info = inst->current_test_info();

      // https://google.github.io/googletest/reference/testing.html#UnitTest::current_test_suite
      std::string test_name_ = info->name();
      std::string test_suite_name_ = info->test_suite_name();
      std::string n = test_suite_name_ + "-" + test_name_;

      cwd = fs_get_cwd();
      ASSERT_FALSE(cwd.empty());

      dir = cwd + "/ffs_test_" + n + "_dir";

      in_dir = "./invalid-memory-trailing-non-null-terminated-string_view";
      nonnull_dir = std::string_view(in_dir.data(), 2);
      ASSERT_NE(nonnull_dir.back(), '\0') << "nonnull_dir should not be null-terminated\n";

    }

    void TearDown() override {
      fs_remove(dir);
    }
  };


TEST_F(TestMkdir, Mkdir){

EXPECT_FALSE(fs_mkdir(""));

// Test mkdir with existing directory
ASSERT_TRUE(fs_mkdir(dir));

// Test mkdir with relative path
ASSERT_TRUE(fs_set_cwd(dir));

ASSERT_TRUE(fs_mkdir("test-filesystem-dir/hello"));
EXPECT_TRUE(fs_is_dir(dir + "/test-filesystem-dir/hello"));

ASSERT_TRUE(fs_mkdir(nonnull_dir));
EXPECT_FALSE(fs_is_dir(in_dir));
}
