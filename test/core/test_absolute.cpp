#include "ffilesystem.h"

#include <gtest/gtest.h>

#include <string>

class TestAbs : public testing::Test {
protected:

  std::string base, ref, cwd;
  std::string sys_drive;

  void SetUp() override {
    cwd = ::testing::UnitTest::GetInstance()->original_working_dir();
    ASSERT_FALSE(cwd.empty()) << "Failed to get current working directory";

    if (fs_is_windows()) {
      base = "j:/foo";
      ref = std::string("j:/foo") + fs_filesep() + "rel";
      auto d = fs_getenv("SystemDrive");
      ASSERT_TRUE(d.has_value()) << "Failed to get SystemDrive";
      sys_drive = d.value();
    } else {
      base = "/foo";
      ref = "/foo/rel";
    }
  }
};


TEST_F(TestAbs, Absolute){

EXPECT_EQ(fs_absolute(""), cwd);
EXPECT_EQ(fs_absolute("", ""), cwd);

EXPECT_EQ(fs_absolute("rel", base), ref);

EXPECT_EQ(fs_absolute(cwd + "/rel"), cwd + "/rel");

// absolute("./rel") may be "/fullpath/./rel" (our method, and most <filesystem> except Windows)
//                     or "/fullpath/rel" (Windows <filesystem>)
// using for base "." or ".." and similar has similar ambiguity for testing.

// relative path, empty base
EXPECT_EQ(fs_absolute("rel", ""), cwd + fs_filesep() + "rel");

// empty path, relative base
EXPECT_EQ(fs_absolute("", "rel"), cwd + fs_filesep() + "rel");

EXPECT_EQ(fs_absolute("日本語"), cwd + fs_filesep() + "日本語");
EXPECT_EQ(fs_absolute("have space"), cwd + fs_filesep() + "have space");

}


TEST_F(TestAbs, Windows){

if(!fs_is_windows())
  GTEST_SKIP() << "Windows only test";

EXPECT_EQ(fs_absolute(R"(\\?\X:\anybody)"), R"(\\?\X:\anybody)");
EXPECT_EQ(fs_absolute(R"(\\?\UNC\server\share)"), R"(\\?\UNC\server\share)");

EXPECT_EQ(fs_absolute(sys_drive + "/"), sys_drive + "/");
}


TEST(IsAbs, Agnostic)
{
EXPECT_FALSE(fs_is_absolute(""));

EXPECT_FALSE(fs_is_absolute("日本語"));
EXPECT_FALSE(fs_is_absolute("some space here"));
}

TEST_F(TestAbs, IsAbsWindows)
{
if(!fs_is_windows())
  GTEST_SKIP() << "Windows only test";

EXPECT_TRUE(fs_is_absolute(sys_drive + "/"));

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
