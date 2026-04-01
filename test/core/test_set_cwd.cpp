#include "ffilesystem.h"
#include <string>

#include <gtest/gtest.h>

class TestSetCwd : public testing::Test {
  protected:
    std::string dir, cwd, in_dir;
    std::string_view nonnull_dir;

    void SetUp() override {
      cwd = ::testing::UnitTest::GetInstance()->original_working_dir();
      dir = cwd + "/ffs_test_set_cwd";
      ASSERT_TRUE(fs_mkdir(dir));
      ASSERT_TRUE(fs_is_dir(dir));

      in_dir = "./invalid-memory-trailing-non-null-terminated-string_view";
      nonnull_dir = std::string_view(in_dir.data(), 2);
      ASSERT_NE(nonnull_dir.back(), '\0') << "nonnull_dir should not be null-terminated\n";
    }
    void TearDown() override {
      fs_set_cwd(cwd);
      fs_remove(dir);
    }
};

TEST_F(TestSetCwd, SetCwd)
{

EXPECT_FALSE(fs_set_cwd(""));

ASSERT_TRUE(fs_set_cwd(dir));

EXPECT_TRUE(fs_equivalent(fs_get_cwd(), dir)) << "cwd " << fs_get_cwd() << " != " << dir << " canonical " << fs_canonical(dir);

EXPECT_TRUE(fs_set_cwd(nonnull_dir)) << "problem with non null-terminated path " << nonnull_dir;
EXPECT_TRUE(fs_equivalent(fs_get_cwd(), dir));
}
