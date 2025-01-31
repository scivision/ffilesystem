// tests for fs_is_appexec_alias()

#include <iostream>
#include <string>
#include <string_view>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"

int main(int argc, char* argv[])
{

if (!fs_is_windows())
  skip("This test is only for Windows");

std::string path;
// not string_view, it garbles on some platforms

if (argc > 1)
  path = argv[1];
else {
  const std::string confdir = fs_user_config_dir();
  if(confdir.empty())
    err("didn't find a User Config directory to test");

  std::string appdir = confdir + "/Microsoft/WindowsApps";
  if (!fs_is_dir(appdir))
    err("didn't find an App Execution Alias directory to test");

  std::cout << "App Execution Alias directory: " << appdir << "\n";

  for (const auto& exe : {"wt.exe", "winget.exe", "wsl.exe", "bash.exe"}){
    path = fs_which(exe, appdir);
    if (!path.empty())
      break;
  }

  if(path.empty()){
    if (fs_getenv("CI") == "true")
      skip("didn't find an App Execution Alias to test");
    else
      err("didn't find an App Execution Alias to test");
  }
}

std::cout << "Testing fs_is_appexec_alias(" << path << ")\n";

if (!fs_is_appexec_alias(path))
  err("fs_is_appexec_alias(" + std::string(path) + ") was not detected as an App Execution Alias");

ok_msg("fs_is_appexec_alias(" + std::string(path) + ")");

return EXIT_SUCCESS;

}
