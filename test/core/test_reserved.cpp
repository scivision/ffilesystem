#include <string>

#include <boost/ut.hpp>

#include "ffilesystem.h"
#include "ffilesystem_test.hpp"

int main() {
using namespace boost::ut;

"reserved_agnostic"_test = [] {
std::string ref = fs_devnull();

expect(!ffilesystem::is_reserved("."));

expect(eq(ffilesystem::normal(ref), ref));

bool b = ffilesystem::is_absolute(ref);
if (ffilesystem::is_windows)
  expect(!b);
else
  expect(b);

b = ffilesystem::is_reserved(ref);
if (ffilesystem::is_windows)
  expect(b);
else
  expect(!b);

expect(!ffilesystem::is_directory(ref));

const auto size = ffilesystem::file_size(ref);
expect(any_of{0UL, fs_unknown_size} == size);

// omitted ffilesystem::space_available() since some systems don't handle NUL /dev/null well
// e.g. Windows, macOS GCC, etc.

expect(eq(ffilesystem::expanduser(ref), ref));

expect(!ffilesystem::copy_file(ref, ref, false));

// touch is ambiguous on reserved, so omit

expect(!ffilesystem::is_symlink(ref));

};

#ifdef _WIN32
skip /
#endif
"reserved_posix"_test = [] {
  std::string ref = fs_devnull();

expect(!ffilesystem::is_exe(ref));

// NOTE: do not test
//
// create_directories(/dev/null)
// remove(/dev/null)
// create_symlink()
// set_permissionss()
//
// since if testing with "root" privileges,
// it can make the system unusable until reboot!

expect(ffilesystem::exists(ref));

expect(!ffilesystem::is_file(ref));

expect(!ffilesystem::canonical(ref, false).empty());
};
}
