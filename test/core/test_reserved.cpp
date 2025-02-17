#include <string>

#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestReserved, Reserved)
{

std::string const ref(fs_devnull());

EXPECT_FALSE(fs_is_reserved("."));

EXPECT_EQ(fs_normal(ref), ref);

bool b = fs_is_absolute(ref);
if (fs_is_windows())
  EXPECT_FALSE(b);
else
  EXPECT_TRUE(b);

b = fs_is_reserved(ref);
if (fs_is_windows())
  EXPECT_TRUE(b);
else
  EXPECT_FALSE(b);

EXPECT_FALSE(fs_is_dir(ref));

EXPECT_EQ(fs_file_size(ref), 0);

// omitted fs_space_available() since some systems don't handle NUL /dev/null well
// e.g. Windows, macOS GCC, etc.


if(!fs_is_windows()){

EXPECT_FALSE(fs_is_exe(ref));

// NOTE: do not test
//
// create_directories(/dev/null)
// remove(/dev/null)
// create_symlink()
// set_permissionss()
//
// since if testing with "root" privilidges,
// it can make the system unusable until reboot!

EXPECT_TRUE(fs_exists(ref));

EXPECT_FALSE(fs_is_file(ref));

EXPECT_FALSE(fs_canonical(ref, false, true).empty());

} // !fs_is_windows()

EXPECT_EQ(fs_expanduser(ref), ref);

EXPECT_FALSE(fs_copy_file(ref, ref, false));

// touch is ambiguous on reserved, so omit

EXPECT_FALSE(fs_is_symlink(ref));

}
