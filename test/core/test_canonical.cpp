#include <string>

#include "ffilesystem.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>


class TestCanonical : public testing::Test {
  protected:
    std::string cwd, cwdp;

    void SetUp() override {
      cwd = ::testing::UnitTest::GetInstance()->original_working_dir();
      cwd = fs_as_posix(cwd);
      ASSERT_FALSE(cwd.empty());

      cwdp = fs_parent(cwd);
      ASSERT_FALSE(cwdp.empty());
    }
};


TEST_F(TestCanonical, CanonicalParentDir)
{
std::string r = fs_canonical("..", true);
ASSERT_FALSE(r.empty());
ASSERT_EQ(r.length(), cwdp.length()) << r << " vs " << cwdp;
EXPECT_TRUE(fs_equivalent(r, cwdp));

r = fs_canonical("..", false);
ASSERT_FALSE(r.empty());
ASSERT_EQ(r.length(), cwdp.length()) << r << " vs " << cwdp;
EXPECT_TRUE(fs_equivalent(r, cwdp));
}
TEST_F(TestCanonical, ResolveParentDir)
{
std::string r = fs_resolve("..", true);
ASSERT_FALSE(r.empty());
ASSERT_EQ(r.length(), cwdp.length()) << r << " vs " << cwdp;
EXPECT_TRUE(fs_equivalent(r, cwdp));

r = fs_resolve("..", false);
ASSERT_FALSE(r.empty());
ASSERT_EQ(r.length(), cwdp.length()) << r << " vs " << cwdp;
EXPECT_TRUE(fs_equivalent(r, cwdp));
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
std::string h = fs_canonical("../" + name, false);
EXPECT_FALSE(h.empty());

EXPECT_GT(h.length(), name.length());
EXPECT_THAT(h, ::testing::EndsWith(name));

std::string r = "日本語";

h = fs_canonical(r, false);

EXPECT_THAT(h, ::testing::EndsWith(r));
}


TEST_F(TestCanonical, Realpath)
{
std::string r = fs_realpath(".");
ASSERT_FALSE(r.empty());
ASSERT_EQ(r.length(), cwd.length());

EXPECT_TRUE(fs_equivalent(r, cwd));
}
