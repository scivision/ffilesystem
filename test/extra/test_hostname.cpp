#include "ffilesystem.h"

#include <vector>
#include <string>

#include <gtest/gtest.h>

TEST(TestHostname, Hostname){
  std::string s = fs_hostname();

  EXPECT_FALSE(s.empty());

  EXPECT_NE(s.length(), fs_get_max_path()) << "hostname length is equal to max path length";
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
