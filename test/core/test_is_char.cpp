#include "ffilesystem.h"
#include <string_view>

#include <gtest/gtest.h>

class TestChar : public testing::Test {
    protected:
      std::string_view p;

      void SetUp() override {
        p = fs_is_windows() ? "NUL" : "/dev/null";
      }
    };

// /dev/stdin may not be available on CI systems

TEST_F(TestChar, IsChar)
{
EXPECT_TRUE(fs_is_char_device(p));
}

TEST_F(TestChar, IsFile)
{
EXPECT_FALSE(fs_is_file(p));
}
