#include <string>
#include <vector>
#include <iostream>

#include "ffilesystem.h"

#include <filesystem>

#include <gtest/gtest.h>


class TestExe : public testing::Test {
  protected:
    std::filesystem::path cwd;
    std::string exe;
    std::string noexe;
    std::string self;

    void SetUp() override {
      auto inst = testing::UnitTest::GetInstance();
      auto info = inst->current_test_info();
      std::string test_name_ = info->name();
      std::string test_suite_name_ = info->test_suite_name();
      std::string n = test_suite_name_ + "-" + test_name_;
      cwd = std::filesystem::current_path();

      if(fs_is_wsl() > 0 && fs_filesystem_type(cwd.string()) == "v9fs")
        GTEST_SKIP() << "v9fs to NTFS etc. doesn't work right";

      exe = "test_" + n + ".exe";
      noexe = "test_" + n + "_noexe.exe";

      std::vector<std::string> argvs = ::testing::internal::GetArgvs();
      self = argvs[0];

      ASSERT_TRUE(fs_touch(exe));
      ASSERT_TRUE(fs_touch(noexe));

      ASSERT_TRUE(fs_set_permissions(exe, 0, 0, 1));
      ASSERT_TRUE(fs_set_permissions(noexe, 0, 0, -1));

      ASSERT_FALSE(fs_get_permissions(exe).empty());
    }

    void TearDown() override {
      fs_remove(exe);
      fs_remove(noexe);
    }
};


TEST_F(TestExe, IsExe){

EXPECT_FALSE(fs_is_exe(""));

EXPECT_FALSE(fs_is_file("not-exist"));
EXPECT_FALSE(fs_is_exe("not-exist"));

EXPECT_TRUE(fs_is_exe(self));
EXPECT_TRUE(fs_is_exe(exe));
}


TEST_F(TestExe, IsExeBin){
EXPECT_TRUE(fs_is_executable_binary(self));
EXPECT_FALSE(fs_is_executable_binary(exe));
EXPECT_FALSE(fs_is_executable_binary(noexe));
}


TEST_F(TestExe, PermsSelf){
  std::string p = fs_get_permissions(self);
  EXPECT_EQ(p[2], 'x');
}


TEST_F(TestExe, IsExePerms){
  std::string p = fs_get_permissions(exe);
  EXPECT_EQ(p[2], 'x');
}


TEST_F(TestExe, IsNotExePerms){
  if(fs_is_windows())
    GTEST_SKIP() << "not available on Windows";

  EXPECT_FALSE(fs_is_exe(noexe));

  std::string p = fs_get_permissions(noexe);
  EXPECT_EQ(p[2], '-');
}


TEST_F(TestExe, ChmodExe){

  std::string p = fs_get_permissions(exe);
  std::cout << "permissions before chmod(" << exe << ", true)  = " << p << "\n";

  ASSERT_TRUE(fs_set_permissions(exe, -1, 0, 1));
  // test executable even without read permission

  p = fs_get_permissions(exe);
  std::cout << "permissions after chmod(" << exe << ", true) = " << p << "\n";

  EXPECT_TRUE(fs_is_exe(exe));
  EXPECT_EQ(p[2], 'x');
}


TEST_F(TestExe, ChmodNoExe){
  if(fs_is_windows())
    GTEST_SKIP() << "not available on Windows";

  std::string p = fs_get_permissions(noexe);
  std::cout << "permissions before chmod(" << noexe << ", false)  = " << p << "\n";

  ASSERT_TRUE(fs_set_permissions(noexe, 0, 0, -1));

  p = fs_get_permissions(noexe);
  std::cout << "permissions after chmod(" << noexe << ",false) = " << p << "\n";

  EXPECT_FALSE(fs_is_exe(noexe));
  EXPECT_EQ(p[2], '-');
}


TEST_F(TestExe, WhichExe){
  EXPECT_EQ(fs_which(exe, cwd.string()), cwd / exe);
}
