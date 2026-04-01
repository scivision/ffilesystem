#include "ffilesystem.h"

#include <gtest/gtest.h>


class TestSymlink : public testing::Test {
  protected:
    std::string tfile, tdir, in_file;
    std::string_view nonnull_file;

    void SetUp() override {
      std::vector<std::string> argvs = ::testing::internal::GetArgvs();
      auto argc = argvs.size();
      ASSERT_GE(argc, 3) << "please give test link file and link dir as arguments";

      tfile = argvs[argc-2];
      ASSERT_TRUE(fs_is_file(tfile)) << tfile << " is not a file";

      tdir = argvs[argc-1];
      ASSERT_TRUE(fs_is_dir(tdir)) << tdir << " is not a directory";

      in_file = tfile + "-read_past_the_end_of_buffer";
      nonnull_file = std::string_view(in_file.data(), tfile.size());
      ASSERT_NE(nonnull_file.back(), '\0');
    }
};


TEST_F(TestSymlink, IsSymlinkFile){

EXPECT_FALSE(fs_is_symlink("not-exist-file"));

EXPECT_FALSE(fs_is_symlink(""));

EXPECT_TRUE(fs_is_symlink(tfile));

EXPECT_TRUE(fs_is_symlink(nonnull_file)) << "is_symlink() should not read past the end of string_view buffer";
}

TEST_F(TestSymlink, IsSymlinkDir){
  EXPECT_TRUE(fs_is_symlink(tdir));
}
