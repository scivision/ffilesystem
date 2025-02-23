#include <cstdio> // for std::remove
#include <ctime>

#include "ffilesystem.h"

#include <gtest/gtest.h>

class TestTouch : public testing::Test {
  protected:
    std::string file;
    void SetUp() override {
      file = "ffs_test_touch.txt";
    }
    void TearDown() override {
      std::remove(file.c_str());
    }
};

TEST_F(TestTouch, Touch)
{

ASSERT_TRUE(fs_touch(file));

auto t0 = fs_get_modtime(file);
EXPECT_GT(t0, 0);

EXPECT_TRUE(fs_set_modtime(file));

EXPECT_GE(fs_get_modtime(file), t0);

EXPECT_FALSE(fs_set_modtime("not-exist-file"));

}
