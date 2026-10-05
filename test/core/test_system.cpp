#include "ffilesystem.h"

#include <boost/ut.hpp>
#ifdef FFS_SYSTEM_DISABLED
#include "disabled_system.h"
#endif

int main() {
  using namespace boost::ut;

  "system"_test = [] {
    expect(!fs_compiler().empty()) << "unknown compiler";
    expect(neq(fs_compiler().length(), fs_get_max_path())) << "compiler has blank space";

#ifdef FFS_SYSTEM_DISABLED
    ffs_test::expect_disabled_string([] { return fs_get_username(); });
#else
    expect(!fs_get_username().empty());
    expect(neq(fs_get_username().length(), fs_get_max_path())) << "username has blank space";
#endif
  };
}
