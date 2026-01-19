#include <string>
#include <cstdint>

#include "ffilesystem.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class TestReserved : public testing::Test {
  protected:
    std::string ref;

    void SetUp() override {
      ref = fs_devnull();
    }
};

TEST_F(TestReserved, Agnostic)
{

constexpr std::uintmax_t e = static_cast<std::uintmax_t>(-1);

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

EXPECT_THAT(fs_file_size(ref), ::testing::AnyOf(0UL, e));

// omitted fs_space_available() since some systems don't handle NUL /dev/null well
// e.g. Windows, macOS GCC, etc.

EXPECT_EQ(fs_expanduser(ref), ref);

EXPECT_FALSE(fs_copy_file(ref, ref, false));

// touch is ambiguous on reserved, so omit

EXPECT_FALSE(fs_is_symlink(ref));

}

TEST_F(TestReserved, Posix)
{
  if(fs_is_windows())
    GTEST_SKIP() << "Test only for POSIX";

EXPECT_FALSE(fs_is_exe(ref));

// NOTE: do not test
//
// create_directories(/dev/null)
// remove(/dev/null)
// create_symlink()
// set_permissionss()
//
// since if testing with "root" privileges,
// it can make the system unusable until reboot!

EXPECT_TRUE(fs_exists(ref));

EXPECT_FALSE(fs_is_file(ref));

EXPECT_FALSE(fs_canonical(ref, false).empty());

}
