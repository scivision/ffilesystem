#include "ffilesystem.h"

#include <gtest/gtest.h>


class TestRemove : public testing::Test {
  protected:
    std::string file;
    void SetUp() override {
      file = testing::TempDir() + "/test_remove.txt";
      ASSERT_TRUE(fs_touch(file));
    }
    void TearDown() override {
      std::remove(file.c_str());
    }
  };


TEST_F(TestRemove, Remove)
{
ASSERT_TRUE(fs_remove(file));
}
