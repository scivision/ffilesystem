#include "ffilesystem.h"
#include <string>

#include <gtest/gtest.h>

class TestRealpath: public testing::Test {
  protected:
    std::string cwd;
    void SetUp() override {
      cwd = fs_get_cwd();
      ASSERT_FALSE(cwd.empty());
    }
};


TEST_F(TestRealpath, Realpath)
{

EXPECT_EQ(fs_realpath("."), cwd);
EXPECT_TRUE(fs_realpath("not-exist-realpath/b/c").empty());

EXPECT_LT(fs_realpath("..").length(), cwd.length());

}
