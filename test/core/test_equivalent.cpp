#include <string>

#include "ffilesystem.h"

#include <gtest/gtest.h>


class TestSame : public testing::Test {
    protected:
      std::string cwd;
      void SetUp() override {
        cwd = fs_as_posix(::testing::UnitTest::GetInstance()->original_working_dir());
      }
};

TEST_F(TestSame, Equivalent)
{
std::string s1 = "../" + fs_file_name(cwd);
std::string s2 = "./" + s1;

ASSERT_TRUE(fs_is_dir(s1));
EXPECT_TRUE(fs_is_dir(s2));

EXPECT_TRUE(fs_equivalent(s1, s2));
EXPECT_TRUE(fs_equivalent("..", fs_parent(cwd)));
EXPECT_TRUE(fs_equivalent(".", "./"));
EXPECT_TRUE(fs_equivalent(".", cwd));

s1 = "ffs_equiv_not-exist";
EXPECT_FALSE(fs_equivalent(s1, s1));

}
