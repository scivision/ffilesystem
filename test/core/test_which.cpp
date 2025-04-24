#include "ffilesystem.h"

#include <gtest/gtest.h>


class TestWhich : public testing::Test {
  protected:
    std::string cwd;
    std::string name;
    std::string self;
    std::string rel;
    std::string testExe;

    void SetUp() override {

      testExe = fs_is_windows() ? "cmake.exe" : "ls";

      cwd = fs_as_posix(::testing::UnitTest::GetInstance()->original_working_dir());

      std::vector<std::string> argvs = ::testing::internal::GetArgvs();
      self = fs_which(argvs[0], cwd);
      ASSERT_FALSE(self.empty());
      ASSERT_TRUE(fs_is_exe(self));

      name = fs_file_name(self);
      rel = std::string("./") + name;
    }
};

class TestWhichNoPath : public testing::Test {
  protected:
    std::string testExe;
    void SetUp() override {
      testExe = fs_is_windows() ? "cmake.exe" : "ls";
      ASSERT_TRUE(fs_setenv("PATH", ""));
    }
};


TEST_F(TestWhich, Which){

  EXPECT_TRUE(fs_is_absolute(fs_which(testExe)));
  EXPECT_EQ(fs_file_name(fs_which(testExe)), testExe);
  EXPECT_TRUE(fs_which(testExe, "nowhere").empty());

  EXPECT_TRUE(fs_which("/not/a/path").empty());
  EXPECT_TRUE(fs_which("").empty());

  // Build a relative path from the running program
  EXPECT_TRUE(fs_is_file(rel));
  EXPECT_FALSE(fs_which(rel).empty());

  EXPECT_TRUE(fs_which("not-exist/" + name).empty());
}


TEST_F(TestWhich, WhichLocalDir){
  ASSERT_TRUE(fs_is_file(name));
  // for Windows only: local dir is preferred
  if (fs_is_windows())
    EXPECT_FALSE(fs_which(name).empty());
  else
    EXPECT_TRUE(fs_which(name).empty());
}


TEST_F(TestWhichNoPath, WhichNoPath){
  EXPECT_TRUE(fs_which(testExe).empty());
  EXPECT_TRUE(fs_which(testExe, "nowhere").empty());
}
