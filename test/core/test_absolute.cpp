#include "ffilesystem.h"

#include <boost/ut.hpp>

#include <string>

int main(){

using namespace boost::ut;

std::string base, ref, cwd, sys_drive;

cwd = fs_get_cwd();
expect(!cwd.empty()) << "Failed to get current working directory";

if (fs_is_windows()) {
  base = "j:/foo";
  ref = std::string("j:/foo") + fs_filesep() + "rel";
  auto d = fs_getenv("SystemDrive");
  expect(d.has_value()) << "Failed to get SystemDrive";
  sys_drive = d.value();
} else {
  base = "/foo";
  ref = "/foo/rel";
}


expect(fs_absolute("") == cwd);
expect(fs_absolute("", "") == cwd);

expect(fs_absolute("rel", base) == ref);

expect(fs_absolute(cwd + "/rel") == (cwd + "/rel"));

// absolute("./rel") may be "/fullpath/./rel" (our method, and most <filesystem> except Windows)
//                     or "/fullpath/rel" (Windows <filesystem>)
// using for base "." or ".." and similar has similar ambiguity for testing.

// relative path, empty base
// relative path, empty base
expect(fs_absolute("rel", "") == cwd + fs_filesep() + "rel");

// empty path, relative base
expect(fs_absolute("", "rel") == cwd + fs_filesep() + "rel");

expect(fs_absolute("日本語") == cwd + fs_filesep() + "日本語");
expect(fs_absolute("have space") == cwd + fs_filesep() + "have space");

// Windows-only checks
if (fs_is_windows()) {
  expect(fs_absolute(R"(\\?\X:\anybody)") == R"(\\?\X:\anybody)");
  expect(fs_absolute(R"(\\?\UNC\server\share)") == R"(\\?\UNC\server\share)");

  expect(fs_absolute(sys_drive + "/") == sys_drive + "/");

  expect(fs_is_absolute(sys_drive + "/"));
  expect(fs_is_absolute("J:/"));
  expect(fs_is_absolute("j:/"));
  expect(!fs_is_absolute("j:"));
  expect(!fs_is_absolute("/"));
  expect(!fs_is_absolute("/日本語"));

  expect(fs_is_absolute(R"(\\?\)"));
  expect(fs_is_absolute(R"(\\.\)"));

  expect(fs_is_absolute(R"(\\?\C:\)"));
  expect(fs_is_absolute(R"(\\.\C:\)"));
  expect(fs_is_absolute(R"(\\?\UNC\server\share)"));
  expect(fs_is_absolute(R"(\\?\UNC\server\share\日本語)"));
  expect(fs_is_absolute(R"(\\server\share\some space here)"));
  expect(fs_is_absolute(R"(\\?\C:\some space here)"));
}

// Agnostic IsAbs checks
expect(!fs_is_absolute(""));
expect(!fs_is_absolute("日本語"));
expect(!fs_is_absolute("some space here"));

// Posix-only checks
if (!fs_is_windows()) {
  expect(fs_is_absolute("/"));
  expect(fs_is_absolute("/日本語"));
  expect(!fs_is_absolute("j:/"));
}


}
