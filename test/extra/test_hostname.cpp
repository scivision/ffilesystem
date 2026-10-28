#include "ffilesystem.h"

#include <string>

#include <boost/ut.hpp>
#ifdef FFS_SYSTEM_DISABLED
#include "disabled_system.h"
#endif

int main() {
using namespace boost::ut;

#ifndef FFS_SYSTEM_DISABLED
bool const is_ci = ffilesystem::getenv("CI").value_or("") == "true";
#endif

"Hostname"_test = [] {
#ifdef FFS_SYSTEM_DISABLED
  ffs_test::expect_disabled_string([] { return ffilesystem::hostname(); });
#else
  std::string s = ffilesystem::hostname();

  expect(!s.empty());

  expect(s.length() != fs_get_max_path()) << "hostname length is equal to max path length";
#endif
};

"MaxComponent"_test = [] {
  expect(ffilesystem::max_component("/") >= 1);
};


#ifdef FFS_SYSTEM_DISABLED
"Shell"_test = [] {
  ffs_test::expect_disabled_string([] { return ffilesystem::get_shell(); });
};

"Terminal"_test = [] {
  ffs_test::expect_disabled_string([] { return ffilesystem::get_terminal(); });
};
#else
"Shell"_test = [is_ci] {
  std::string s = ffilesystem::get_shell();

  if (!is_ci)
    expect(!s.empty()) << "shell is empty";

  if (!s.empty())
    expect(s.length() != fs_get_max_path()) << "shell has blank space";
};


"Terminal"_test = [] {
  // agents or CI runners may not have a terminal attached
  std::string s = ffilesystem::get_terminal();

  if (!s.empty())
    expect(s.length() != fs_get_max_path()) << "terminal has blank space";
};
#endif
}
