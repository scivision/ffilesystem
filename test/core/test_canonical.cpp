#include <string>

#include "ffilesystem.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>


class TestCanonical : public testing::Test {
  protected:
    std::string home, homep, cwd;

    void SetUp() override {
      cwd = fs_as_posix(::testing::UnitTest::GetInstance()->original_working_dir());
      home = fs_get_homedir();
      homep = fs_parent(home);
      // NOTE: if root user, might be that homep == home == "/"

      ASSERT_FALSE(home.empty());
      ASSERT_FALSE(homep.empty());
    }
};

TEST_F(TestCanonical, Tilde)
{
EXPECT_EQ(fs_canonical("~", true, true), home);
EXPECT_EQ(fs_canonical("~", false, true), home);
EXPECT_THAT(fs_canonical("~", false, false), ::testing::EndsWith("~"));
}


TEST_F(TestCanonical, ParentDir)
{
EXPECT_EQ(fs_canonical("~/..", true, true), homep);
EXPECT_EQ(fs_canonical("~/..", false, true), homep);
EXPECT_THAT(fs_canonical("~/..", false, false), ::testing::AnyOf(".", cwd));
}

TEST_F(TestCanonical, ParentRel)
{
EXPECT_EQ(fs_canonical("../not-exist", false), fs_parent(cwd) + "/not-exist");
}


TEST_F(TestCanonical, RelativeFile)
{

if(fs_is_cygwin())
  GTEST_SKIP() << "Cygwin can't handle non-existing canonical paths";

std::string name = "ffs_not-exist_cpp.txt";
std::string h = fs_canonical("~/../" + name, false, true);
EXPECT_FALSE(h.empty());

EXPECT_GT(h.length(), name.length());
EXPECT_THAT(h, ::testing::EndsWith(name));

std::string r = "日本語";

h = fs_canonical(r, false, true);

EXPECT_THAT(h, ::testing::EndsWith(r));
}
