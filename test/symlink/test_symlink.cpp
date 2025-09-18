#include "ffilesystem.h"

#include <gtest/gtest.h>

class TestSymlink : public testing::Test {
  protected:
    std::string cwd, tgt, link, link_dir;

    void SetUp() override {
      auto inst = testing::UnitTest::GetInstance();
      auto info = inst->current_test_info();
      std::string test_name_ = info->name();
      std::string test_suite_name_ = info->test_suite_name();
      std::string n = test_suite_name_ + "-" + test_name_;

      cwd = fs_get_cwd();
      ASSERT_FALSE(cwd.empty()) << "get_cwd() should not return empty string";
      tgt = cwd + fs_filesep() + "test_" + n + "_cpp.txt";

      ASSERT_TRUE(fs_touch(tgt));
      ASSERT_TRUE(fs_is_file(tgt)) << "is_file(" << tgt << ") should be true for existing regular file";

      link = cwd + fs_filesep() + "test_" + n + "_cpp.link";
      link_dir = cwd + fs_filesep() + "test_" + n + "_cpp.dir.link";

      if (fs_is_symlink(link)){
        ASSERT_TRUE(fs_remove(link));
      }

      if (fs_is_symlink(link_dir)){
        ASSERT_TRUE(fs_remove(link_dir));
      }

    ASSERT_TRUE(fs_create_symlink(tgt, link));
    ASSERT_TRUE(fs_create_symlink(cwd, link_dir));
    }

    void TearDown() override {
      fs_remove(tgt);
      if (fs_is_symlink(link))
        fs_remove(link);
      if (fs_is_symlink(link_dir))
        fs_remove(link_dir);
    }
};


TEST_F(TestSymlink, CreateSymlink){

  EXPECT_FALSE(fs_create_symlink(tgt, "")) << "create_symlink() should fail with empty link";

  ASSERT_FALSE(fs_is_symlink(tgt)) << "is_symlink() should be false for non-symlink file: " << tgt;

  EXPECT_FALSE(fs_create_symlink("", link)) << "create_symlink() should fail with empty target";


  EXPECT_TRUE(fs_is_symlink(link)) << "is_symlink() should be true for symlink: " << link;
  EXPECT_TRUE(fs_is_file(link)) << "is_file(" << link << ") should be true for existing regular file target " << tgt;
  EXPECT_EQ(fs_read_symlink(link), tgt);
  // Cygwin will have /cygdrive/c and /home/ as roots
  if (!fs_is_cygwin()){
    EXPECT_EQ(fs_canonical(link, true, false), tgt);
  }

  EXPECT_TRUE(fs_read_symlink(tgt).empty());
  EXPECT_TRUE(fs_read_symlink("not-exist-file").empty());
  EXPECT_FALSE(fs_is_symlink(cwd));


  EXPECT_TRUE(fs_is_dir(link_dir)) << "is_dir(" << link_dir << ") should be true for link to existing dir";
  EXPECT_TRUE(fs_is_symlink(link_dir)) << "is_symlink() should be true for symlink: " << link_dir;
}
