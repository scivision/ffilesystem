#include "ffilesystem.h"

#include <iostream>

#include <gtest/gtest.h>

class TestSymlink : public testing::Test {
  protected:
    std::string cwd, tgt, link_file, link_dir, broken_link, not_exist_tgt, in_file, in2;
    std::string_view nonnull_file;

    void SetUp() override {
      auto inst = testing::UnitTest::GetInstance();
      auto info = inst->current_test_info();
      std::string test_name_ = info->name();
      std::string test_suite_name_ = info->test_suite_name();
      std::string n = test_suite_name_ + "-" + test_name_;

      cwd = fs_realpath(inst->original_working_dir());
      // realpath is for Windows Dev Drive and Networked drives
      ASSERT_FALSE(cwd.empty()) << "get_cwd() should not return empty string";
      tgt = cwd + fs_filesep() + "test_" + n + "_cpp.txt";
      not_exist_tgt = cwd + fs_filesep() + "test_" + n + "_cpp.notexist";

      ASSERT_TRUE(fs_touch(tgt));
      ASSERT_TRUE(fs_is_file(tgt)) << "is_file(" << tgt << ") should be true for existing regular file";

      link_file = cwd + fs_filesep() + "test_" + n + "_cpp.link";
      link_dir = cwd + fs_filesep() + "test_" + n + "_cpp.dir.link";
      broken_link = cwd + fs_filesep() + "test_" + n + "_cpp.broken";
      in_file = link_file + "-in_file";
      in2 = in_file + "-read_past_the_end_of_buffer";

      for (const auto& link : {link_file, link_dir, broken_link, in_file, in2}){
        if (fs_is_symlink(link)){
          std::cout << "Removing existing symlink: " << link << "\n";
          ASSERT_TRUE(fs_remove(link)) << "Failed to remove existing symlink: " << link;
        }
      }

    ASSERT_TRUE(fs_create_symlink(tgt, link_file));
    std::cout << "Created symlink FILE: " << link_file << " -> " << tgt << "\n";
    ASSERT_TRUE(fs_create_symlink(cwd, link_dir));
    std::cout << "Created symlink DIR: " << link_dir << " -> " << cwd << "\n";

    // to create a broken link, we first create a valid link and then remove the target
    ASSERT_TRUE(fs_touch(not_exist_tgt));
    ASSERT_TRUE(fs_create_symlink(not_exist_tgt, broken_link));
    ASSERT_TRUE(fs_remove(not_exist_tgt));
    ASSERT_FALSE(fs_exists(not_exist_tgt)) << "exists() should be false for non-existent target: " << not_exist_tgt;
    std::cout << "Created broken symlink: " << broken_link << " -> " << not_exist_tgt << "\n";

    nonnull_file = std::string_view(in2.data(), in_file.size());
    ASSERT_NE(nonnull_file.back(), '\0');
    }

    void TearDown() override {
      for (const auto& link : {tgt, link_file, link_dir, broken_link, in_file, in2}){
        fs_remove(link);
      }
    }
};


TEST_F(TestSymlink, CreateSymlink){

  EXPECT_FALSE(fs_create_symlink(tgt, "")) << "create_symlink() should fail with empty link";

  ASSERT_FALSE(fs_is_symlink(tgt)) << "is_symlink() should be false for non-symlink file: " << tgt;

  EXPECT_FALSE(fs_create_symlink("", link_file)) << "create_symlink() should fail with empty target";

  ASSERT_TRUE(fs_create_symlink(tgt, nonnull_file)) << "create_symlink(" << tgt << ", " << nonnull_file << ") should succeed even if link path is not null-terminated";
  EXPECT_TRUE(fs_exists(in_file)) << "exists() should be false for non-existent file: " << in_file;


  EXPECT_TRUE(fs_is_symlink(link_file)) << "is_symlink() should be true for symlink: " << link_file;
  EXPECT_TRUE(fs_is_file(link_file)) << "is_file(" << link_file << ") should be true for existing regular file target " << tgt;
  EXPECT_EQ(fs_read_symlink(link_file), tgt);
  // Cygwin will have /cygdrive/c and /home/ as roots
  if (!fs_is_cygwin()){
    std::string r = fs_canonical(link_file, true);
    ASSERT_FALSE(r.empty());
    ASSERT_EQ(r.length(), tgt.length()) << r << " vs " << tgt;
    EXPECT_TRUE(fs_equivalent(r, tgt));
  }

  EXPECT_TRUE(fs_read_symlink(tgt).empty());
  EXPECT_TRUE(fs_read_symlink(not_exist_tgt).empty());
  EXPECT_FALSE(fs_is_symlink(cwd));

  EXPECT_TRUE(fs_is_dir(link_dir)) << "is_dir(" << link_dir << ") should be true for link to existing dir";
  EXPECT_TRUE(fs_is_symlink(link_dir)) << "is_symlink() should be true for symlink: " << link_dir;

  EXPECT_EQ(fs_read_symlink(nonnull_file), tgt) << "read_symlink() should not read past the end of string_view buffer";
}


TEST_F(TestSymlink, Exists){
  EXPECT_TRUE(fs_exists(tgt)) << "exists() should be true for existing file: " << tgt;
  EXPECT_TRUE(fs_exists(link_file)) << "exists() should be true for existing symlink: " << link_file;
  EXPECT_TRUE(fs_exists(link_dir)) << "exists() should be true for existing symlink: " << link_dir;

  if (!fs_is_windows() || (fs_is_msvc() && fs_backend() == "<filesystem>")) {
    // Python os.stat() and os.lstat() handle broken symlink correctly on Windows
    EXPECT_FALSE(fs_exists(broken_link)) << "exists() should be false for broken symlink: " << broken_link;
  }
}


TEST_F(TestSymlink, Lexists){
  EXPECT_FALSE(fs_lexists(not_exist_tgt)) << "lexists() should be false for non-existent target: " << not_exist_tgt;
  EXPECT_FALSE(fs_lexists("")) << "lexists() should be false for empty path";
  EXPECT_TRUE(fs_lexists(tgt)) << "lexists() should be true for existing file: " << tgt;
  EXPECT_TRUE(fs_lexists(link_file)) << "lexists() should be true for existing symlink: " << link_file;
  EXPECT_TRUE(fs_lexists(link_dir)) << "lexists() should be true for existing symlink: " << link_dir;
  EXPECT_TRUE(fs_lexists(broken_link)) << "lexists() should be true for broken symlink: " << broken_link;
}
