#include <string>
#include <string_view>

#include "ffilesystem.h"

#include <gtest/gtest.h>


class TestEquivalent : public testing::Test {
    protected:
      std::string self, self_name;
      std::string_view nonnull_file;
      std::string in_file; // this must be in global scope for nonnull_file to be valid

      void SetUp() override {
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

if (!fs_equivalent(".", fs_parent(self)))
  GTEST_SKIP() << "Test executable is not in the current working directory";

ASSERT_TRUE(fs_is_file(self_name)) << "Test executable name not found in CWD: " << self_name;

EXPECT_TRUE(fs_equivalent(self_name, "./" + self_name));

EXPECT_TRUE(fs_equivalent(self_name, self));
EXPECT_TRUE(fs_equivalent(self_name, nonnull_file));
EXPECT_TRUE(fs_equivalent(self, self_name));
EXPECT_TRUE(fs_equivalent(self, self));
}


TEST(TestEquiv, Relative)
{

std::string s = "ffs_equiv_not-exist";
EXPECT_FALSE(fs_equivalent(s, s));

std::string cwd = ::testing::UnitTest::GetInstance()->original_working_dir();

EXPECT_TRUE(fs_equivalent("..", fs_parent(cwd)));
EXPECT_TRUE(fs_equivalent(".", "./"));
EXPECT_TRUE(fs_equivalent(".", cwd));
EXPECT_FALSE(fs_equivalent("..", cwd));

// NOTE: This can be false on networked drive or Windows Dev Drive
// fs_equivalent(".", fs_realpath("."));
}


TEST(TestEquiv, InaccessibleDirectory)
{
if(fs_is_windows() || fs_is_cygwin()){
  GTEST_SKIP() << "Permission-denied directory traversal semantics are POSIX-specific";
}

if(fs_is_admin()){
  GTEST_SKIP() << "Administrator/root can bypass permission checks";
}

if(!fs_is_writable(".")){
  GTEST_SKIP() << "current directory is not writable";
}

std::string base = "ffs_equiv_inaccessible_dir";
std::string secret = base + "/secret";

// Clean up stale state from prior interrupted runs.
if(fs_exists(secret)){
  fs_set_permissions(secret, 1, 1, 1);
}
if(fs_exists(base)){
  fs_remove(secret);
  fs_remove(base);
}

ASSERT_TRUE(fs_mkdir(secret));
ASSERT_TRUE(fs_set_permissions(base, -1, -1, -1));

// test that associated functions also work
EXPECT_FALSE(fs_exists(secret)) << "inaccessible path treated as not existing";
EXPECT_FALSE(fs_is_dir(secret)) << "inaccessible path should not be treated as directory";
EXPECT_FALSE(fs_is_file(secret)) << "inaccessible path should not be treated as file";

EXPECT_FALSE(fs_equivalent(secret, secret));

EXPECT_TRUE(fs_set_permissions(base, 1, 1, 1));
EXPECT_TRUE(fs_remove(secret));
EXPECT_TRUE(fs_remove(base));
}
