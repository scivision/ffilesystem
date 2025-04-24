#include "ffilesystem.h"

#include <vector>
#include <string>

#include <gtest/gtest.h>

TEST(TestHostname, Hostname){
  std::string s = fs_hostname();

  EXPECT_FALSE(s.empty());

  EXPECT_NE(s.length(), fs_get_max_path()) << "hostname length is equal to max path length";
}

TEST(TestExe, ExePath){
  std::string p = fs_exe_path();
  EXPECT_FALSE(p.empty());

  // TARGET_FILE_NAME to allow for macOS possible non-canonical result or
  // symlinked / special filesystems like WSL
  std::string cwd = fs_as_posix(::testing::UnitTest::GetInstance()->original_working_dir());

  std::vector<std::string> argvs = ::testing::internal::GetArgvs();
  std::string self = fs_which(argvs[0], cwd);

  std::string n = fs_file_name(p);
  EXPECT_TRUE(fs_equivalent(p, self)) << "executable path is not equivalent to the current executable";
}


TEST(TestMax, MaxComponent){
  EXPECT_GE(fs_max_component("/"), 1);
}


TEST(TestShell, Shell){
  std::string s = fs_get_shell();
  if (s.empty())
    GTEST_SKIP() << "unknown shell";

  EXPECT_NE(s.length(), fs_get_max_path()) << "shell has blank space";
}


TEST(TestShell, Terminal){
  std::string s = fs_get_terminal();

  if (s.empty())
    GTEST_SKIP() << "unknown terminal";

  EXPECT_NE(s.length(), fs_get_max_path()) << "terminal has blank space";
}
