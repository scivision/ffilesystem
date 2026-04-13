#include <string>
#include <string_view>

#include "ffilesystem.h"

#include <gtest/gtest.h>


class TestEquivalent : public testing::Test {
    protected:
      std::string cwd, self, self_name, in_file;
      std::string_view nonnull_file;

      void SetUp() override {
        cwd = ::testing::UnitTest::GetInstance()->original_working_dir();
        std::vector<std::string> argvs = ::testing::internal::GetArgvs();

        self = argvs[0];
        self_name = fs_file_name(self);

        ASSERT_TRUE(fs_is_file(self)) << "Test executable not found: " << self;

        in_file = self + "-read_past_the_end_of_buffer";
        nonnull_file = std::string_view(in_file.data(), self.size());
        ASSERT_NE(nonnull_file.back(), '\0') << "Test executable name should not end with null character";
      }
};

TEST_F(TestEquivalent, FileName)
{

if (!fs_equivalent(cwd, fs_parent(self)))
  GTEST_SKIP() << "Test executable is not in the current working directory";

ASSERT_TRUE(fs_is_file(self_name)) << "Test executable name not found in CWD: " << self_name;

EXPECT_TRUE(fs_equivalent(self_name, "./" + self_name));

EXPECT_TRUE(fs_equivalent(self_name, self));
EXPECT_TRUE(fs_equivalent(self_name, nonnull_file));
EXPECT_TRUE(fs_equivalent(self, self_name));
EXPECT_TRUE(fs_equivalent(self, self));
}

TEST_F(TestEquivalent, Relative)
{

std::string s = "ffs_equiv_not-exist";
EXPECT_FALSE(fs_equivalent(s, s));

EXPECT_TRUE(fs_equivalent("..", fs_parent(cwd)));
EXPECT_TRUE(fs_equivalent(".", "./"));
EXPECT_TRUE(fs_equivalent(".", cwd));
EXPECT_FALSE(fs_equivalent("..", cwd));

// NOTE: This can be false on networked drive or Windows Dev Drive
// fs_equivalent(".", fs_realpath("."));
}
