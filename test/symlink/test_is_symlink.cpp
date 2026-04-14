#include "ffilesystem.h"
#include <iostream>
#include <gtest/gtest.h>


class TestSymlink : public testing::Test {
  protected:
    std::string tfile, tdir, in_file, tgt, test_dir;
    std::string_view nonnull_file;

    void SetUp() override {
      std::string cwd = testing::UnitTest::GetInstance()->original_working_dir();

      test_dir = cwd + "/symlink_test_dir";
      ASSERT_TRUE(fs_mkdir(test_dir)) << "Failed to create test directory: " << test_dir;

      tgt = test_dir + "/test_is_symlink_target.txt";
      ASSERT_TRUE(fs_touch(tgt)) << "Failed to create target file: " << tgt;

      tdir = test_dir + "/link.dir";
      tfile = tdir + "/cmake_test_symlink.txt.link";

      if(fs_is_symlink(tfile)){
        std::cout << "Removing existing test symlink file: " << tfile << "\n";
        ASSERT_TRUE(fs_remove(tfile)) << "Failed to remove existing test symlink file: " << tfile;
      }

      if(fs_is_symlink(tdir)){
        std::cout << "Removing existing test symlink dir: " << tdir << "\n";
        ASSERT_TRUE(fs_remove(tdir)) << "Failed to remove existing test symlink dir: " << tdir;
      }

      ASSERT_TRUE(fs_create_symlink(test_dir, tdir)) << "Failed to create symlink: " << tdir << " -> " << test_dir;
      ASSERT_TRUE(fs_is_dir(tdir)) << tdir << " is not a directory";
      std::cout << "Created symlink DIR: " << tdir << " -> " << test_dir << "\n";

      ASSERT_TRUE(fs_create_symlink(tgt, tfile)) << "Failed to create symlink: " << tfile << " -> " << tgt;
      ASSERT_TRUE(fs_is_file(tfile)) << tfile << " is not a file after creating symlink";
      std::cout << "Created symlink FILE: " << tfile << " -> " << tgt << "\n";

      in_file = tfile + "-read_past_the_end_of_buffer";
      nonnull_file = std::string_view(in_file.data(), tfile.size());
      ASSERT_NE(nonnull_file.back(), '\0');
    }

    void TearDown() override {
      for (const auto& link : {tfile, tdir}){
        if (fs_is_symlink(link))
          fs_remove(link);
      }

      fs_remove(tgt);
      fs_remove(test_dir);
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
