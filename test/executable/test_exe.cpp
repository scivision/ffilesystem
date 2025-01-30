#include <iostream>
#include <cstdlib>
#include <string>
#include <string_view>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main()
{

#ifdef _MSC_VER
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

auto cwd = fs_get_cwd();
if(cwd.empty())
  return EXIT_FAILURE;

if(fs_is_wsl() > 0 && fs_filesystem_type(cwd) == "v9fs")
  skip("exe: v9fs to NTFS etc. doesn't work right");

std::string exe = "test_executable_cpp.exe";
std::string noexe = "test_not_executable_cpp.exe";

// Empty string
if(fs_is_exe(""))
  err("test_exe: is_exe('') should be false");

// Non-existent file
if (fs_is_file("not-exist"))
  err("test_exe: not-exist-file should not exist.");
if (fs_is_exe("not-exist"))
  err("test_exe: not-exist-file cannot be executable");
auto p = fs_get_permissions("not-exist");
if(!p.empty())
  err("test_exe: get_permissions('not-exist') should fail");

fs_touch(exe);
fs_touch(noexe);

fs_set_permissions(exe, 0, 0, 1);
fs_set_permissions(noexe, 0, 0, -1);

p = fs_get_permissions(exe);
if(p.empty())
  err("test_exe: get_permissions(" + exe + ") failed");

std::cout << "permissions: " << exe << " = " << p << "\n";

if(p[2] != 'x')
  err("test_exe: expected perms for " + exe + " to be 'x' in index 2 but got " + p);

if (!fs_is_windows() && !fs_is_exe(exe))
  err("test_exe: " + exe + " is not executable and should be.");

p = fs_get_permissions(noexe);
if(p.empty())
  err("test_exe: get_permissions(" + noexe + ") failed");
std::cout << "permissions: " << noexe << " = " << p << "\n";

if (fs_is_exe(noexe)){
  if(fs_is_windows()){
    std::cerr << "XFAIL:Windows: test_exe: is_exe() did not detect non-executable file " << noexe << " on Windows\n";
  }
  else{
   err("test_exe: " + noexe + " is executable and should not be.");
  }
}

// chmod setup

fs_remove(exe);
fs_remove(noexe);

// chmod(true)
fs_touch(exe);
if (!fs_is_file(exe))
  err("test_exe: " + exe + " is not a file.");

p = fs_get_permissions(exe);
if(p.empty())
  err("test_exe: get_permissions(" + exe + ") failed");

std::cout << "permissions before chmod(" << exe << ", true)  = " << p << "\n";

fs_set_permissions(exe, -1, 0, 1);
// test executable even without read permission

p = fs_get_permissions(exe);
if(p.empty())
  err("test_exe: get_permissions(" + exe + ") failed after set");
std::cout << "permissions after chmod(" << exe << ", true) = " << p << "\n";

if (!fs_is_exe(exe)){
  if(fs_is_windows()){
    std::cerr << "XFAIL:Windows: test_exe: is_exe() did not detect executable file " << exe << " on Windows\n";
  }
  else{
    err("test_exe: is_exe() did not detect executable file " + exe);
  }
}

if (!fs_is_windows() && p[2] != 'x'){
  err("test_exe: expected POSIX perms for " + exe + " to be 'x' in index 2");
}

// chmod(false)
fs_touch(noexe);
if (!fs_is_file(noexe))
  err("test_exe: " + noexe + " is not a file.");

p = fs_get_permissions(noexe);
if(p.empty())
  err("test_exe: get_permissions(" + noexe + ") failed after touch");
std::cout << "permissions before chmod(" << noexe << ", false)  = " << p << "\n";

fs_set_permissions(noexe, 0, 0, -1);

p = fs_get_permissions(noexe);
if(p.empty())
  err("test_exe: get_permissions(" + noexe + ") failed after set");

std::cout << "permissions after chmod(" << noexe << ",false) = " << p << "\n";

if(!fs_is_windows())
{
  if (fs_is_exe(noexe))
    err("test_exe: did not detect non-executable file.");

  if (p[2] != '-')
    err("test_exe: expected POSIX perms for " + noexe + " to be '-' in index 2");
}

// test which
std::string_view name = (fs_is_windows()) ? "cmake.exe" : "ls";

std::string r = fs_which(name);
if(r.length() == 0)
  std::cerr << "ERROR:test_exe: which(" << name << ") should return a path. This can happen on CI systems.\n";

std::cout << "which(" << name << ") " << r << "\n";

// our own chmod(exe)
r = fs_which(exe);
if(r.length() == 0)
  err("test_exe: which(" + exe + ") should return a path.");

std::cout << "which(" << exe << ") " << r << "\n";


fs_remove(exe);
fs_remove(noexe);

ok_msg("is_exe C++");

return EXIT_SUCCESS;
}
