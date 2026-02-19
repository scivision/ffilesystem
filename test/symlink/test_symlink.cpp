#include "ffilesystem.h"

#include <iostream>

#include <gtest/gtest.h>

class TestSymlink : public testing::Test {
  protected:
    std::string cwd, tgt, link_file, link_dir, broken_link, not_exist_tgt;

    void SetUp() override {
      auto inst = testing::UnitTest::GetInstance();
      auto info = inst->current_test_info();
      std::string test_name_ = info->name();
      std::string test_suite_name_ = info->test_suite_name();
      std::string n = test_suite_name_ + "-" + test_name_;

      cwd = inst->original_working_dir();
      ASSERT_FALSE(cwd.empty()) << "get_cwd() should not return empty string";
      tgt = cwd + fs_filesep() + "test_" + n + "_cpp.txt";
      not_exist_tgt = cwd + fs_filesep() + "test_" + n + "_cpp.notexist";

      ASSERT_TRUE(fs_touch(tgt));
      ASSERT_TRUE(fs_is_file(tgt)) << "is_file(" << tgt << ") should be true for existing regular file";

      link_file = cwd + fs_filesep() + "test_" + n + "_cpp.link";
      link_dir = cwd + fs_filesep() + "test_" + n + "_cpp.dir.link";
      broken_link = cwd + fs_filesep() + "test_" + n + "_cpp.broken";

      if (fs_is_symlink(link_file)){
        ASSERT_TRUE(fs_remove(link_file));
      }

      if (fs_is_symlink(link_dir)){
        ASSERT_TRUE(fs_remove(link_dir));
      }

    ASSERT_TRUE(fs_create_symlink(tgt, link_file));
    std::cout << "Created symlink FILE: " << link_file << " -> " << tgt << "\n";
    ASSERT_TRUE(fs_create_symlink(cwd, link_dir));
    std::cout << "Created symlink DIR: " << link_dir << " -> " << cwd << "\n";

    // to create a broken link, we first create a valid link and then remove the target
    ASSERT_TRUE(fs_touch(not_exist_tgt));
    ASSERT_TRUE(fs_create_symlink(not_exist_tgt, broken_link));
    ASSERT_TRUE(fs_remove(not_exist_tgt));
    std::cout << "Created broken symlink: " << broken_link << " -> " << not_exist_tgt << "\n";
    }

    void TearDown() override {
      fs_remove(tgt);
      if (fs_is_symlink(link_file))
        fs_remove(link_file);
      if (fs_is_symlink(link_dir))
        fs_remove(link_dir);
      fs_remove(broken_link);
    }
};


TEST_F(TestSymlink, CreateSymlink){

  EXPECT_FALSE(fs_create_symlink(tgt, "")) << "create_symlink() should fail with empty link";

  ASSERT_FALSE(fs_is_symlink(tgt)) << "is_symlink() should be false for non-symlink file: " << tgt;

  EXPECT_FALSE(fs_create_symlink("", link_file)) << "create_symlink() should fail with empty target";


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
}


TEST_F(TestSymlink, Exists){
  EXPECT_TRUE(fs_exists(tgt)) << "exists() should be true for existing file: " << tgt;
  EXPECT_TRUE(fs_exists(link_file)) << "exists() should be true for existing symlink: " << link_file;
  EXPECT_TRUE(fs_exists(link_dir)) << "exists() should be true for existing symlink: " << link_dir;
  EXPECT_FALSE(fs_exists(broken_link)) << "exists() should be false for broken symlink: " << broken_link;
}


TEST_F(TestSymlink, Lexists){
  EXPECT_FALSE(fs_lexists(not_exist_tgt)) << "lexists() should be false for non-existent target: " << not_exist_tgt;
  EXPECT_FALSE(fs_lexists("")) << "lexists() should be false for empty path";
  EXPECT_TRUE(fs_lexists(tgt)) << "lexists() should be true for existing file: " << tgt;
  EXPECT_TRUE(fs_lexists(link_file)) << "lexists() should be true for existing symlink: " << link_file;
  EXPECT_TRUE(fs_lexists(link_dir)) << "lexists() should be true for existing symlink: " << link_dir;
  EXPECT_TRUE(fs_lexists(broken_link)) << "lexists() should be true for broken symlink: " << broken_link;
}