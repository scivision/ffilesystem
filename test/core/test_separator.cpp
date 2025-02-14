#include <iostream>
#include <cstdlib>
#include <string>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ffilesystem.h"
#include "ffilesystem_test.h"

void test_as_posix(){

  std::string p;

  if(!fs_as_posix(p).empty())
    err("test_as_posix: " + p);

  if(fs_is_windows()){
    p = "a\\b";
    if(fs_as_posix(p) != "a/b")
      err("test_as_posix: " + fs_as_posix(p) + " != a/b");

    p = R"(C:\my\path)";
    if(fs_as_posix(p) != "C:/my/path")
      err("test_as_posix: " + p);
  }

  std::cout << "OK: as_posix\n";
}


int main() {

#ifdef _MSC_VER
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

  test_as_posix();

  if(fs_is_windows() && fs_pathsep() != ';')
    err("pathsep windows");
  if(!fs_is_windows() && fs_pathsep() != ':')
    err("pathsep unix");

  return EXIT_SUCCESS;
}
