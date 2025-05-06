#include "ffilesystem.h"

#include <gtest/gtest.h>

class TestAbs : public testing::Test {
protected:
  std::string base, ref;
  std::string cwd;
  void SetUp() override {
    cwd = fs_as_posix(::testing::UnitTest::GetInstance()->original_working_dir());
    if (fs_is_windows()) {
      base = "j:/foo";
      ref = "j:/foo/rel";
    } else {
      base = "/foo";
      ref = "/foo/rel";
    }
  }
};


TEST_F(TestAbs, Absolute){

EXPECT_EQ(fs_absolute("", false), cwd);
EXPECT_EQ(fs_absolute("", "", false), cwd);

EXPECT_EQ(fs_absolute("rel", base, false), ref);

EXPECT_EQ(fs_absolute(cwd + "/rel", false), cwd + "/rel");

// absolute("./rel") may be "/fullpath/./rel" (our method, and most <filesystem> except Windows)
//                     or "/fullpath/rel" (Windows <filesystem>)
// using for base "." or ".." and similar has similar ambiguity for testing.

// relative path, empty base
EXPECT_EQ(fs_absolute("rel", "", false), cwd + "/rel");

// empty path, relative base
EXPECT_EQ(fs_absolute("", "rel", false), cwd + "/rel");

EXPECT_EQ(fs_absolute("日本語", false), cwd + "/日本語");
EXPECT_EQ(fs_absolute("have space", false), cwd + "/have space");

}


TEST_F(TestAbs, Tilde){
std::string home = fs_get_homedir();

EXPECT_EQ(fs_absolute("~", false), cwd + "/~");
EXPECT_EQ(fs_absolute("~/", true), home);

EXPECT_EQ(fs_absolute("~/a", false), cwd + "/~/a");
EXPECT_EQ(fs_absolute("~/a", true), home + "/a");
}


TEST(IsAbs, Agnostic)
{
EXPECT_FALSE(fs_is_absolute(""));

EXPECT_FALSE(fs_is_absolute("日本語"));
EXPECT_FALSE(fs_is_absolute("some space here"));
}

TEST(IsAbs, Windows)
{
if(!fs_is_windows())
  GTEST_SKIP() << "Windows only test";

EXPECT_TRUE(fs_is_absolute("J:/"));
EXPECT_TRUE(fs_is_absolute("j:/"));
EXPECT_FALSE(fs_is_absolute("j:"));
EXPECT_FALSE(fs_is_absolute("/"));
EXPECT_FALSE(fs_is_absolute("/日本語"));

EXPECT_TRUE(fs_is_absolute(R"(\\?\)"));
EXPECT_TRUE(fs_is_absolute(R"(\\.\)"));

EXPECT_TRUE(fs_is_absolute(R"(\\?\C:\)"));
EXPECT_TRUE(fs_is_absolute(R"(\\.\C:\)"));
EXPECT_TRUE(fs_is_absolute(R"(\\?\UNC\server\share)"));
EXPECT_TRUE(fs_is_absolute(R"(\\?\UNC\server\share\日本語)"));
EXPECT_TRUE(fs_is_absolute(R"(\\server\share\some space here)"));
EXPECT_TRUE(fs_is_absolute(R"(\\?\C:\some space here)"));
}

TEST(IsAbs, Posix)
{

if(fs_is_windows())
  GTEST_SKIP() << "Posix only test";

EXPECT_TRUE(fs_is_absolute("/"));
EXPECT_TRUE(fs_is_absolute("/日本語"));
EXPECT_FALSE(fs_is_absolute("j:/"));
}
