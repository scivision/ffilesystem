#include "ffilesystem.h"

#include <gtest/gtest.h>

class TestAppExec : public testing::Test {
  protected:
    std::string path;

    void SetUp() override {

      if(!fs_is_windows())
        GTEST_SKIP() << "requires Windows";

      std::string appdir = fs_getenv("LOCALAPPDATA").value_or("") + "/Microsoft/WindowsApps";
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
