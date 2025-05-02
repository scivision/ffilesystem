#include <string>

#include "ffilesystem.h"

#include <gtest/gtest.h>


class TestSame : public testing::Test {
    protected:
      std::string cwd;
      std::string self_name;
      void SetUp() override {
        cwd = fs_as_posix(::testing::UnitTest::GetInstance()->original_working_dir());
        std::vector<std::string> argvs = ::testing::internal::GetArgvs();
        self_name = fs_file_name(argvs[0]);
      }
};

TEST_F(TestSame, FileName)
{
EXPECT_TRUE(fs_equivalent(self_name, "./" + self_name));
}

TEST_F(TestSame, NotExist)
{
std::string s = "ffs_equiv_not-exist";
EXPECT_FALSE(fs_equivalent(s, s));
}


TEST_F(TestSame, Relative)
{
EXPECT_TRUE(fs_equivalent("..", fs_parent(cwd)));
EXPECT_TRUE(fs_equivalent(".", "./"));
EXPECT_TRUE(fs_equivalent(".", cwd));
}
