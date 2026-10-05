#include "ffilesystem.h"
#include <string>

#include <boost/ut.hpp>
#ifdef FFS_SYSTEM_DISABLED
#include "disabled_system.h"
#endif

int main() {
using namespace boost::ut;

std::string cenv = fs_getenv("CI").value_or("");

"owner_name"_test = [] {
  expect(!fs_get_owner_name(".").empty());
};

if (cenv != "true") {
"owner_group"_test = [] {
  expect(!fs_get_owner_group(".").empty());
};
}

// mismatched username and owner can happen

"username"_test = [] {
#ifdef FFS_SYSTEM_DISABLED
  ffs_test::expect_disabled_string([] { return fs_get_username(); });
#else
  expect(!fs_get_username().empty());
#endif
};
}
