#include <string>

#include "ffilesystem.h"

#include <gtest/gtest.h>


class TestSame : public testing::Test {
    protected:
      std::string cwd, self, self_name;
      void SetUp() override {
        cwd = ::testing::UnitTest::GetInstance()->original_working_dir();
        std::vector<std::string> argvs = ::testing::internal::GetArgvs();

        self = argvs[0];
        self_name = fs_file_name(self);

        ASSERT_TRUE(fs_is_file(self)) << "Test executable not found: " << self;
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
EXPECT_FALSE(fs_equivalent("..", cwd));
}
