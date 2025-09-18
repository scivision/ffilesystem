#include <string>

#include "ffilesystem.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>


class TestCanonical : public testing::Test {
  protected:
    std::string home, homep, cwd, cwdp;

    void SetUp() override {
      cwd = fs_as_posix(fs_get_cwd());
      ASSERT_FALSE(cwd.empty());

      cwdp = fs_parent(cwd);
      ASSERT_FALSE(cwdp.empty());

      home = fs_get_homedir();
      homep = fs_parent(home);
      // NOTE: if root user, might be that homep == home == "/"

      ASSERT_FALSE(home.empty());
      ASSERT_FALSE(homep.empty());
    }
};

TEST_F(TestCanonical, CanonicalTilde)
{
EXPECT_EQ(fs_canonical("~", true, true), home);
EXPECT_EQ(fs_canonical("~", false, true), home);
EXPECT_THAT(fs_canonical("~", false, false), ::testing::EndsWith("~"));
}
TEST_F(TestCanonical, ResolveTilde)
{
EXPECT_EQ(fs_resolve("~", true, true), home);
EXPECT_EQ(fs_resolve("~", false, true), home);
EXPECT_THAT(fs_resolve("~", false, false), ::testing::EndsWith("~"));
}


TEST_F(TestCanonical, CanonicalParentDir)
{
EXPECT_EQ(fs_canonical("~/..", true, true), homep);
EXPECT_EQ(fs_canonical("~/..", false, true), homep);
}
TEST_F(TestCanonical, ResolveParentDir)
{
EXPECT_EQ(fs_resolve("~/..", true, true), homep);
EXPECT_EQ(fs_resolve("~/..", false, true), homep);
}


TEST(TestResolve, CanonicalWindows){
if(!fs_is_windows())
  GTEST_SKIP() << "Windows specific test";

auto d = fs_getenv("SystemDrive");
ASSERT_TRUE(d.has_value()) << "Failed to get SystemDrive";
std::string sys_drive = d.value();

EXPECT_THAT(fs_canonical(sys_drive + "/", true), ::testing::AnyOf(sys_drive + "\\", sys_drive + "/"));
EXPECT_THAT(fs_canonical(sys_drive + "/", false), ::testing::AnyOf(sys_drive + "\\", sys_drive + "/"));

EXPECT_THAT(fs_canonical("M:/", false), ::testing::AnyOf("M:\\", "M:/"));

if(fs_backend() != "<filesystem>"){

EXPECT_THAT(fs_canonical(R"(\\?\)" + sys_drive + "\\", true),
             ::testing::AnyOf(R"(\\?\)" + sys_drive + "\\", R"(\\?\)" + sys_drive + "/"));
}

}

TEST(TestResolve, ResolveWindows){
if(!fs_is_windows())
  GTEST_SKIP() << "Windows specific test";

auto d = fs_getenv("SystemDrive");
ASSERT_TRUE(d.has_value()) << "Failed to get SystemDrive";
std::string sys_drive = d.value();

EXPECT_THAT(fs_resolve(sys_drive + "/", true), ::testing::AnyOf(sys_drive + "\\", sys_drive + "/"));
EXPECT_THAT(fs_resolve(sys_drive + "/", false), ::testing::AnyOf(sys_drive + "\\", sys_drive + "/"));

if(fs_backend() != "<filesystem>"){

EXPECT_THAT(fs_resolve(R"(\\?\)" + sys_drive + "\\", true),
             ::testing::AnyOf(R"(\\?\)" + sys_drive + "\\", R"(\\?\)" + sys_drive + "/"));
}

}


TEST_F(TestCanonical, CanonicalParentRel)
{
EXPECT_THAT(fs_canonical("../not-exist", false), ::testing::AnyOf("../not-exist", cwdp + "/not-exist"));
EXPECT_THAT(fs_canonical("./not-exist", false), ::testing::AnyOf("not-exist", cwd + "/not-exist"));
EXPECT_THAT(fs_canonical("a/b/../c", false), ::testing::AnyOf("a/c", cwd + "/a/c"));
}
TEST_F(TestCanonical, ResolveParentRel)
{
EXPECT_EQ(fs_resolve("../not-exist", false), cwdp + "/not-exist");
EXPECT_EQ(fs_resolve("./not-exist", false), cwd + "/not-exist");
EXPECT_EQ(fs_resolve("a/b/../c", false), cwd + "/a/c");
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


TEST_F(TestCanonical, Realpath)
{
EXPECT_EQ(fs_realpath("."), cwd);
}
