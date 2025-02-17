#include <string>

#include "ffilesystem.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(TestCanonical, Canonical)
{

std::string home = fs_get_homedir();
ASSERT_FALSE(home.empty());

EXPECT_EQ(fs_canonical("~", true, true), home);

std::string homep = fs_parent(home);
ASSERT_FALSE(homep.empty());

// -- relative dir

EXPECT_EQ(fs_canonical("~/..", true, true), homep);

// -- relative file
if(!fs_is_cygwin()){
  // Cygwin can't handle non-existing canonical paths

std::string h = fs_canonical("~/../not-exist.txt", false, true);
EXPECT_FALSE(h.empty());

EXPECT_GT(h.length(), 13);

std::string r = "日本語";

h = fs_canonical(r, false, true);

EXPECT_THAT(h, ::testing::HasSubstr(r));
}

}
