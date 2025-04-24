#include <string>
#include <fstream> // std::ofstream

#include "ffilesystem.h"

#include <gtest/gtest.h>

class TestFilesize : public testing::Test {
  protected:
    std::string file;
    void SetUp() override {
      file = "ffs_filesize_5bytes.txt";
      std::ofstream ofs(file);
      ofs << "hello";
      ofs.close();
    }
    void TearDown() override {
      fs_remove(file);
    }
};


TEST_F(TestFilesize, Filesize)
{
EXPECT_EQ(fs_file_size(file), 5);
}
