#include <string>
#include <fstream>

#include "ffilesystem.h"

#include <gtest/gtest.h>

class TestFileSize : public testing::Test {
  protected:
    std::string file;
    void SetUp() override {
      file = "ffs_filesize_5bytes.txt";
      std::ofstream ofs(file);
      ofs << "hello";
    }
    void TearDown() override {
      fs_remove(file);
    }
};


TEST_F(TestFileSize, FileSize)
{
EXPECT_EQ(fs_file_size(file), 5);

EXPECT_EQ(fs_file_size("not-exist-file"), static_cast<std::uintmax_t>(-1)) << "backend " << fs_backend();
}
