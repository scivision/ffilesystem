#include "ffilesystem.h"
#include <string>

#include <gtest/gtest.h>

class TestSetCwd : public testing::Test {
  protected:
    std::string dir, cwd;
    void SetUp() override {
      cwd = ::testing::UnitTest::GetInstance()->original_working_dir();
      dir = cwd + "/ffs_test_set_cwd";
      ASSERT_TRUE(fs_mkdir(dir));
      ASSERT_TRUE(fs_is_dir(dir));
    }
    void TearDown() override {
      fs_set_cwd(cwd);
      fs_remove(dir);
    }
};

TEST_F(TestSetCwd, SetCwd)
{

EXPECT_FALSE(fs_set_cwd(""));

EXPECT_TRUE(fs_set_cwd(dir));

EXPECT_TRUE(fs_equivalent(fs_get_cwd(), dir)) << "cwd " << fs_get_cwd() << " != " << dir << " canonical " << fs_canonical(dir);

}
