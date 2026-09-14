#include "ffilesystem.h"

#include <vector>
#include <string>

#include <boost/ut.hpp>

int main() {
using namespace boost::ut;

bool const is_ci = fs_getenv("CI").value_or("") == "true";

"Hostname"_test = [] {
  std::string s = fs_hostname();

  expect(!s.empty());

  expect(s.length() != fs_get_max_path()) << "hostname length is equal to max path length";
};

"MaxComponent"_test = [] {
  expect(fs_max_component("/") >= 1);
};


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
}
