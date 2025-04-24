#include "ffilesystem.h"

#include <gtest/gtest.h>

class TestAppExec : public testing::Test {
  protected:
    std::string cwd;
    std::string self;
    std::string confdir;
    std::string path;

    void SetUp() override {

      if(!fs_is_windows())
        GTEST_SKIP() << "requires Windows";

      cwd = fs_as_posix(::testing::UnitTest::GetInstance()->original_working_dir());

      std::vector<std::string> argvs = ::testing::internal::GetArgvs();
      self = fs_which(argvs[0], cwd);

      confdir = fs_user_config_dir();
      if(confdir.empty())
        GTEST_SKIP() << "didn't find a User Config directory to test";

      std::string appdir = confdir + "/Microsoft/WindowsApps";
      ASSERT_TRUE(fs_is_dir(appdir));

      for (const auto& exe : {"wt.exe", "winget.exe", "wsl.exe", "bash.exe"}){
        path = fs_which(exe, appdir);
        if (!path.empty())
          break;
      }
      if(path.empty())
        GTEST_SKIP() << "didn't find an App Execution Alias to test";
    }
};


TEST_F(TestAppExec, AppExecAlias) {
EXPECT_TRUE(fs_is_appexec_alias(path));
}
