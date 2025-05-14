#include "ffilesystem.h"

#include <filesystem>

#include <gtest/gtest.h>

class TestSymlink : public testing::Test {
  protected:
    std::filesystem::path cwd, tgt, link, link_dir;

    void SetUp() override {
      auto inst = testing::UnitTest::GetInstance();
      auto info = inst->current_test_info();
      std::string test_name_ = info->name();
      std::string test_suite_name_ = info->test_suite_name();
      std::string n = test_suite_name_ + "-" + test_name_;

      cwd = std::filesystem::current_path();
      tgt = cwd / ("test_" + n + "_cpp.txt");

      ASSERT_TRUE(fs_touch(tgt.string()));
      ASSERT_TRUE(fs_is_file(tgt.string())) << "is_file(" << tgt << ") should be true for existing regular file";

      link = cwd / ("test_" + n + "_cpp.link");
      link_dir = cwd / ("test_" + n + "_cpp.dir.link");

      if (fs_is_symlink(link.string())){
        ASSERT_TRUE(std::filesystem::remove(link));
      }

      if (fs_is_symlink(link_dir.string())){
        ASSERT_TRUE(std::filesystem::remove(link_dir));
      }

    ASSERT_TRUE(fs_create_symlink(tgt.string(), link.string()));
    ASSERT_TRUE(fs_create_symlink(cwd.string(), link_dir.string()));
    }

    void TearDown() override {
      std::filesystem::remove(tgt);
      if (fs_is_symlink(link.string()))
        std::filesystem::remove(link);
      if (fs_is_symlink(link_dir.string()))
        std::filesystem::remove(link_dir);
    }
};


TEST_F(TestSymlink, CreateSymlink){

  EXPECT_FALSE(fs_create_symlink(tgt.string(), "")) << "create_symlink() should fail with empty link";

  ASSERT_FALSE(fs_is_symlink(tgt.string())) << "is_symlink() should be false for non-symlink file: " << tgt;

  EXPECT_FALSE(fs_create_symlink("", link.string())) << "create_symlink() should fail with empty target";


  EXPECT_TRUE(fs_is_symlink(link.string())) << "is_symlink() should be true for symlink: " << link;
  EXPECT_TRUE(fs_is_file(link.string())) << "is_file(" << link << ") should be true for existing regular file target " << tgt;
  EXPECT_EQ(fs_read_symlink(link.string()), tgt);
  // Cygwin will have /cygdrive/c and /home/ as roots
  if (!fs_is_cygwin()){
    EXPECT_EQ(fs_canonical(link.string(), true, false), tgt);
  }

  EXPECT_TRUE(fs_read_symlink(tgt.string()).empty());
  EXPECT_TRUE(fs_read_symlink("not-exist-file").empty());
  EXPECT_FALSE(fs_is_symlink(cwd.string()));


  EXPECT_TRUE(fs_is_dir(link_dir.string())) << "is_dir(" << link_dir << ") should be true for link to existing dir";
  EXPECT_TRUE(fs_is_symlink(link_dir.string())) << "is_symlink() should be true for symlink: " << link_dir;
}
