#include <string>

#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestWindows, ShortLong)
{

std::string long_path = fs_getenv("PROGRAMFILES");
std::string short_path = fs_shortname(long_path);

ASSERT_FALSE(long_path.empty());
ASSERT_FALSE(short_path.empty());

EXPECT_EQ(fs_longname(short_path), long_path);

}
