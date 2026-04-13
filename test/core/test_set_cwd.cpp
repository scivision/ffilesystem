#include "ffilesystem.h"
#include <string>

#include <gtest/gtest.h>

class TestSetCwd : public testing::Test {
  protected:
    std::string tmp, in_dir;
    std::string_view nonnull_dir;

    void SetUp() override {
      tmp = ::testing::TempDir();
      ASSERT_TRUE(fs_is_dir(tmp));

      in_dir = "./invalid-memory-trailing-non-null-terminated-string_view";
      nonnull_dir = std::string_view(in_dir.data(), 2);
      ASSERT_NE(nonnull_dir.back(), '\0') << "nonnull_dir should not be null-terminated\n";
    }
};

TEST_F(TestSetCwd, SetCwd)
{

EXPECT_FALSE(fs_set_cwd(""));

ASSERT_TRUE(fs_set_cwd(tmp));
// needs to be fs_equivalent due to links, network drives, etc.
EXPECT_TRUE(fs_equivalent(fs_get_cwd(), tmp)) << "cwd " << fs_get_cwd() << " != " << tmp << " canonical " << fs_canonical(tmp);

ASSERT_TRUE(fs_set_cwd(nonnull_dir)) << "problem with non null-terminated path " << nonnull_dir;
EXPECT_TRUE(fs_equivalent(fs_get_cwd(), tmp));
}
