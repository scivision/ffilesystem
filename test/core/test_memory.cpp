#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestMemory, FreeMemory){
EXPECT_GT(fs_get_free_memory(), 0);
}

TEST(TestMemory, TotalMemory){
EXPECT_GT(fs_total_sys_memory(), 0);
}

TEST(TestMemory, FreeLessThanTotal){
EXPECT_LE(fs_get_free_memory(), fs_total_sys_memory());
}
