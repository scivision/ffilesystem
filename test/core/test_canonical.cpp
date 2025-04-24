#include <string>

#include "ffilesystem.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>


class TestCanonical : public testing::Test {
  protected:
    std::string home;

    void SetUp() override {
      home = fs_get_homedir();
      ASSERT_FALSE(home.empty());
    }
};

TEST_F(TestCanonical, Tilde)
{
EXPECT_EQ(fs_canonical("~", true, true), home);
}


TEST_F(TestCanonical, ParentDir)
{
EXPECT_EQ(fs_canonical("~/..", true, true), fs_parent(home));
}


TEST_F(TestCanonical, RelativeFile)
{

if(fs_is_cygwin())
  GTEST_SKIP() << "Cygwin can't handle non-existing canonical paths";

std::string name = "ffs_not-exist_cpp.txt";
std::string h = fs_canonical("~/../" + name, false, true);
EXPECT_FALSE(h.empty());

EXPECT_GT(h.length(), name.length());
EXPECT_THAT(h, ::testing::HasSubstr(name));

std::string r = "日本語";

h = fs_canonical(r, false, true);

EXPECT_THAT(h, ::testing::HasSubstr(r));
}
