#include "ffilesystem.h"
#include <string>

#include <gtest/gtest.h>

TEST(TestExpand, Expanduser)
{
std::string r;
std::string const h = fs_get_homedir();

ASSERT_TRUE(fs_expanduser("").empty());
EXPECT_EQ(fs_expanduser("."), ".");
EXPECT_EQ(fs_expanduser("~"), h);
EXPECT_EQ(fs_expanduser("~/"), h);
EXPECT_EQ(fs_expanduser("~//"), h);
EXPECT_EQ(fs_expanduser("~//test"), h + "/test");
EXPECT_EQ(fs_expanduser("~test"), "~test");
EXPECT_EQ(fs_expanduser("test~"), "test~");
EXPECT_EQ(fs_expanduser("test~test"), "test~test");

EXPECT_EQ(fs_expanduser("日本語"), "日本語");

}
