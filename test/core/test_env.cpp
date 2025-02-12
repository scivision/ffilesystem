#include <iostream>
#include <cstdlib>
#include <string>

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

auto fpath = fs_get_cwd();
if(fpath.empty())
  return EXIT_FAILURE;

std::cout << "current working dir " << fpath << "\n";

if(!fs_exists(fpath))
  err("current working dir " + fpath + " does not exist");

if(!fs_is_dir(fpath))
  err("current working dir " + fpath + " is not a directory");

const auto cpath = fs_get_cwd();
if(cpath.empty())
  return EXIT_FAILURE;

if (std::string s = fs_normal(cpath); fpath != s)
  err("C cwd " + s + " != Fortran cwd " + fpath);

std::cout << "PASS: C++ current working directory\n";

std::string pdir = fs_get_profile_dir();
if(pdir.empty())
  err("failed to get_profile_dir()");
std::cout << "Profile directory " << pdir << "\n";

std::string cdir = fs_user_config_dir();
if(cdir.empty())
  err("failed to get_user_config_dir()");
std::cout << "User config directory " << cdir << "\n";


std::string user = fs_get_username();
if(user.empty())
  err("failed to get_username()");
std::cout << "Username " << user << "\n";

std::string p = fs_get_homedir();
if(p.empty())
  err("failed to get_homedir()");
std::cout << "Home directory " << p << "\n";
if(!fs_is_dir(p))
  err("Fortran: home dir " + p + " does not exist");

// NOTE: profiledir does not always (but may) equal homedir in general, for example when root user.

if (p != fs_expanduser("~"))
  err("home dir " + p + " != expanduser('~') " + fs_expanduser("~"));

std::cout << "PASS: C++ home directory\n";

// --- tempdir
auto t = fs_get_tempdir();
if(t.empty())
  err("failed to get_tempdir()");

std::cout << "Temp directory " << t << "\n";
if (!fs_exists(t))
  err("Fortran: temp dir " + t + " does not exist");

std::cout << "PASS: C++ temp directory\n";

// --- setenv
std::string k = "FORTtest";
std::string v = "FORTvalue";
fs_setenv(k, v);

if (std::string e = fs_getenv(k); e != v)
  err("Fortran: getenv " + k + "=" + e + " != " + v);

ok_msg("getenv tempdir setenv C++");

return EXIT_SUCCESS;
}
