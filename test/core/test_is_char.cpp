#include "ffilesystem.h"
#include <string>

#include <gtest/gtest.h>


// /dev/stdin may not be available on CI systems

TEST(TestChar, IsChar)
{
std::string const p = fs_is_windows() ? "NUL" : "/dev/null";

EXPECT_TRUE(fs_is_char_device(p));

EXPECT_FALSE(fs_is_file(p));


}
