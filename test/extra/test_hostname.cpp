#include "ffilesystem.h"

#include <string>

#include <boost/ut.hpp>
#ifdef FFS_SYSTEM_DISABLED
#include "disabled_system.h"
#endif

int main() {
using namespace boost::ut;

#ifndef FFS_SYSTEM_DISABLED
bool const is_ci = fs_getenv("CI").value_or("") == "true";
#endif

"Hostname"_test = [] {
#ifdef FFS_SYSTEM_DISABLED
  ffs_test::expect_disabled_string([] { return fs_hostname(); });
#else
  std::string s = fs_hostname();

  expect(!s.empty());

  expect(s.length() != fs_get_max_path()) << "hostname length is equal to max path length";
#endif
};

"MaxComponent"_test = [] {
  expect(fs_max_component("/") >= 1);
};


#ifdef FFS_SYSTEM_DISABLED
"Shell"_test = [] {
  ffs_test::expect_disabled_string([] { return fs_get_shell(); });
};

"Terminal"_test = [] {
  ffs_test::expect_disabled_string([] { return fs_get_terminal(); });
};
#else
"Shell"_test = [is_ci] {
  std::string s = fs_get_shell();

  if (!is_ci)
    expect(!s.empty()) << "shell is empty";

  if (!s.empty())
    expect(s.length() != fs_get_max_path()) << "shell has blank space";
};


"Terminal"_test = [is_ci] {
  std::string s = fs_get_terminal();

  if (!is_ci)
    expect(!s.empty()) << "terminal is empty";

  if (!s.empty())
    expect(s.length() != fs_get_max_path()) << "terminal has blank space";
};
#endif
}
