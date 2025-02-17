#include "ffilesystem.h"
#include <string>

#include <gtest/gtest.h>

TEST(TestOwner, Owner){

std::string name = fs_get_username();
EXPECT_FALSE(name.empty());

std::string owner = fs_get_owner_name(testing::TempDir());
EXPECT_FALSE(owner.empty());

EXPECT_FALSE(fs_get_owner_group(testing::TempDir()).empty());

if (fs_getenv("CI") != "true"){
// mismatched username and owner can happen on CI systems
  EXPECT_EQ(name, owner);
}

}
