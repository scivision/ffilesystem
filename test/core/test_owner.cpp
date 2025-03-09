#include "ffilesystem.h"
#include <string>

#include <gtest/gtest.h>

TEST(TestOwner, OwnerName){
EXPECT_FALSE(fs_get_owner_name(".").empty());
}

TEST(TestOwner, OwnerGroup){
EXPECT_FALSE(fs_get_owner_group(".").empty());
}

// mismatched username and owner can happen

TEST(TestOwner, Username){
EXPECT_FALSE(fs_get_username().empty());
}
