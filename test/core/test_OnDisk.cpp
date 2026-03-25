#include "ffilesystem.h"

#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace {
class TestOnDisk : public testing::Test {
  protected:
    std::string file, dir, cwd, sys_drive, in2;
    std::string_view nonnull2;

    void SetUp() override {
      auto inst = testing::UnitTest::GetInstance();
      auto info = inst->current_test_info();

      // https://google.github.io/googletest/reference/testing.html#UnitTest::current_test_suite
      std::string test_name_ = info->name();
      std::string test_suite_name_ = info->test_suite_name();
      std::string n = test_suite_name_ + "-" + test_name_;

      cwd = fs_get_cwd();
      ASSERT_FALSE(cwd.empty());

      if (fs_is_windows()) {
        auto d = fs_getenv("SystemDrive");
        ASSERT_TRUE(d.has_value()) << "Failed to get SystemDrive";
        sys_drive = d.value();
      } else {
        sys_drive = "/";
      }

      file = cwd + "/ffs_" + n + ".txt";
      ASSERT_TRUE(fs_touch(file));
      ASSERT_TRUE(fs_is_file(file));

      dir = cwd + "/ffs_" + n + "_dir";
      ASSERT_TRUE(fs_mkdir(dir));
      ASSERT_TRUE(fs_is_dir(dir));

      in2 = "./invalid-memory-trailing-non-null-terminated-string_view";
      nonnull2 = std::string_view(in2.data(), 2);
      ASSERT_NE(nonnull2.back(), '\0') << "nonnull2 should not be null-terminated\n";
    }
    void TearDown() override {
      fs_set_cwd(cwd);
      fs_remove(file);
      fs_remove(dir);
    }
  };


TEST_F(TestOnDisk, Exists)
{
  EXPECT_TRUE(fs_exists(file));
  EXPECT_TRUE(fs_exists(dir));
  EXPECT_FALSE(fs_exists("ffs_exists_not-exist-file"));
  EXPECT_FALSE(fs_exists(""));

  EXPECT_TRUE(fs_exists(nonnull2)) << "fs_exists(" << nonnull2 << ") should be true\n";
}

TEST_F(TestOnDisk, IsDir)
{
  EXPECT_FALSE(fs_is_dir(""));
  EXPECT_TRUE(fs_is_dir("."));
  EXPECT_TRUE(fs_is_dir(cwd));
  EXPECT_TRUE(fs_is_dir(dir));
  EXPECT_FALSE(fs_is_dir(file));
  EXPECT_FALSE(fs_is_dir("ffs_is_dir_not-exist-dir"));
}


TEST_F(TestOnDisk, IsFile){
EXPECT_TRUE(fs_is_file(file));
EXPECT_FALSE(fs_is_exe(file));
EXPECT_FALSE(fs_is_file("ffs_is_file_not-exist-file"));
EXPECT_FALSE(fs_is_file(""));
EXPECT_FALSE(fs_is_file("."));
EXPECT_FALSE(fs_is_file(dir));
EXPECT_FALSE(fs_is_file(cwd));
}

TEST_F(TestOnDisk, IsReadable)
{
EXPECT_TRUE(fs_is_readable("."));
EXPECT_TRUE(fs_is_readable(file));
EXPECT_TRUE(fs_is_readable(dir));

if(fs_is_windows()){
  EXPECT_TRUE(fs_is_readable(sys_drive));
}

if(fs_win32_long_paths_enabled()){
  EXPECT_TRUE(fs_is_readable(R"(\\?\)" + sys_drive + "\\"));
}

EXPECT_TRUE(fs_is_readable("/"));

EXPECT_TRUE(fs_is_readable(nonnull2)) << "fs_is_readable(" << nonnull2 << ") should be true";

}

TEST_F(TestOnDisk, IsWritable)
{
EXPECT_TRUE(fs_is_writable(file));
EXPECT_TRUE(fs_is_writable(dir));

if(fs_win32_long_paths_enabled()){
  std::string s = fs_as_windows(R"(\\?\)" + fs_canonical(file));
  EXPECT_TRUE(fs_is_writable(s)) << s;
}

EXPECT_TRUE(fs_is_writable(nonnull2)) << "fs_is_writable(" << nonnull2 << ") should be true";

}


TEST_F(TestOnDisk, IsOther){
  EXPECT_FALSE(fs_is_other(""));
  EXPECT_FALSE(fs_is_other("."));
  EXPECT_FALSE(fs_is_other(file));
  EXPECT_FALSE(fs_is_other(dir));
  EXPECT_FALSE(fs_is_other(cwd));
  EXPECT_FALSE(fs_is_other("ffs_is_other_not-exist-file"));
}


TEST_F(TestOnDisk, StatMode){
  EXPECT_NE(fs_st_mode(file), 0);
  EXPECT_NE(fs_st_mode(dir), 0);
  EXPECT_EQ(fs_st_mode("ffs_stat_mode_not-exist-file"), 0);
  EXPECT_EQ(fs_st_mode(""), 0);

  EXPECT_NE(fs_st_mode(nonnull2), 0) << "fs_st_mode(" << nonnull2 << ") should not be 0\n";
}


TEST_F(TestOnDisk, Mkdir){

EXPECT_FALSE(fs_mkdir(""));

// Test mkdir with existing directory
ASSERT_TRUE(fs_mkdir(dir));

// Test mkdir with relative path
ASSERT_TRUE(fs_set_cwd(dir));

ASSERT_TRUE(fs_mkdir("test-filesystem-dir/hello"));
EXPECT_TRUE(fs_is_dir(dir + "/test-filesystem-dir/hello"));
}


TEST_F(TestOnDisk, Realpath){

std::string expected = fs_realpath(cwd);
ASSERT_FALSE(expected.empty());

std::string r = fs_realpath(".");
ASSERT_FALSE(r.empty());
EXPECT_EQ(r, expected) << r << " vs " << expected;

EXPECT_TRUE(fs_realpath("not-exist-realpath/b/c").empty());

r = fs_realpath("..");
ASSERT_FALSE(r.empty());
EXPECT_EQ(r, fs_parent(expected)) << r;
}


TEST_F(TestOnDisk, Touch)
{

EXPECT_TRUE(fs_touch(file));

auto t0 = fs_get_modtime(file);
EXPECT_GT(t0, 0);

EXPECT_TRUE(fs_set_modtime(file));

EXPECT_GE(fs_get_modtime(file), t0);

EXPECT_FALSE(fs_set_modtime("not-exist-file"));

}

TEST_F(TestOnDisk, FilesystemType){
  std::string t = fs_filesystem_type(sys_drive);

  if (t.empty())
      GTEST_SKIP() << "Unknown filesystem type, see type ID in stderr to update fs_get_type()";
  }

}


TEST(TestWindows, ShortLong)
{

if(!fs_is_windows())
  GTEST_SKIP() << "Test only for Windows";

auto e = fs_getenv("PROGRAMFILES");
ASSERT_TRUE(e.has_value()) << "Failed to get PROGRAMFILES environment variable";
std::string long_path = e.value();
ASSERT_FALSE(long_path.empty());

EXPECT_EQ(fs_longname(fs_shortname(long_path)), long_path);

}
