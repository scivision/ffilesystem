#include <string>
#include <fstream>

#include "ffilesystem.h"

#include <gtest/gtest.h>

class TestFileSize : public testing::Test {
  protected:
    std::string file, in_file;
    std::string_view nonnull_file;

    void SetUp() override {
      if(!fs_is_writable(".")){
        GTEST_SKIP() << "current directory is not writable";
      }

      file = "ffs_filesize_5bytes.txt";
      std::ofstream ofs(file);
      ofs << "hello";

      in_file = file + "-read_past_the_end_of_buffer";
      nonnull_file = std::string_view(in_file.data(), file.size());
      ASSERT_NE(nonnull_file.back(), '\0');
    }
    void TearDown() override {
      fs_remove(file);
    }
};


TEST_F(TestFileSize, FileSize)
{
EXPECT_EQ(fs_file_size(file), 5);

EXPECT_EQ(fs_file_size("."), static_cast<std::uintmax_t>(-1)) << "backend " << fs_backend();
EXPECT_EQ(fs_file_size("not-exist-file"), static_cast<std::uintmax_t>(-1)) << "backend " << fs_backend();

EXPECT_EQ(fs_file_size(nonnull_file), 5) << "fs_file_size() non-null-terminated path";
}
