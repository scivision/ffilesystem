#include "ffilesystem.h"

#include <gtest/gtest.h>


class TestSymlink : public testing::Test {
  protected:
    std::string tfile;
    std::string tdir;

    void SetUp() override {
      std::vector<std::string> argvs = ::testing::internal::GetArgvs();
      ASSERT_GE(argvs.size(), 3) << "please give test link file and link dir as arguments";

      int argc = argvs.size();

      tfile = argvs[argc-2];
      ASSERT_TRUE(fs_is_file(tfile)) << tfile << " is not a file";

      tdir = argvs[argc-1];
      ASSERT_TRUE(fs_is_dir(tdir)) << tdir << " is not a directory";
    }
};


TEST_F(TestSymlink, IsSymlinkFile){

EXPECT_FALSE(fs_is_symlink("not-exist-file"));

EXPECT_FALSE(fs_is_symlink(""));

EXPECT_TRUE(fs_is_symlink(tfile));
}

TEST_F(TestSymlink, IsSymlinkDir){
  EXPECT_TRUE(fs_is_symlink(tdir));
}
