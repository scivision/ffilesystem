#include <string>
#include <iostream>

#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestSame, Equivalent)
{
std::string const cwd = fs_get_cwd();
std::cout << "cwd: " << cwd << "\n";
std::string s1 = "../" + fs_file_name(cwd);
std::cout << "cwd relative: " << s1 << "\n";
std::string s2 = "./" + s1;

ASSERT_TRUE(fs_is_dir(s1));
EXPECT_TRUE(fs_is_dir(s2));

EXPECT_TRUE(fs_equivalent(s1, s2));
EXPECT_TRUE(fs_equivalent("..", fs_parent(cwd)));
EXPECT_TRUE(fs_equivalent(".", "./"));
EXPECT_TRUE(fs_equivalent(".", cwd));

s1 = "ffs_equiv_not-exist";
EXPECT_FALSE(fs_equivalent(s1, s1));

}
