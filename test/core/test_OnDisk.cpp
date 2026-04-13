#include "ffilesystem.h"

#include <string>
#include <string_view>

#include <gtest/gtest.h>

class TestOnDisk : public testing::Test {
  protected:
    std::string file, dir, cwd, sys_drive, in_dir, in_sys_dir, in_file;
    std::string_view nonnull_dir, nonnull_file, nonnull_sys_drive;

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

      in_dir = "./invalid-memory-trailing-non-null-terminated-string_view";
      nonnull_dir = std::string_view(in_dir.data(), 2);
      ASSERT_NE(nonnull_dir.back(), '\0') << "nonnull_dir should not be null-terminated\n";

      in_sys_dir = sys_drive + "/invalid-memory-trailing-non-null-terminated-string_view";
      nonnull_sys_drive = std::string_view(sys_drive.data(), sys_drive.size());
      ASSERT_NE(nonnull_sys_drive.back(), '\0') << "nonnull_sys_drive should not be null-terminated\n";

      in_file = file + "-invalid-memory-trailing-non-null-terminated-string_view";
      nonnull_file = std::string_view(in_file.data(), file.size());
      ASSERT_NE(nonnull_file.back(), '\0') << "nonnull_file should not be null-terminated\n";
    }
    void TearDown() override {
      fs_remove(file);
    }
  };


TEST_F(TestOnDisk, Exists)
{
  EXPECT_TRUE(fs_exists(file));
  EXPECT_TRUE(fs_exists(cwd));
  EXPECT_FALSE(fs_exists("ffs_exists_not-exist-file"));
  EXPECT_FALSE(fs_exists(""));

  EXPECT_TRUE(fs_exists(nonnull_dir));
}

TEST_F(TestOnDisk, IsDir)
{
  EXPECT_FALSE(fs_is_dir(""));
  EXPECT_TRUE(fs_is_dir("."));
  EXPECT_TRUE(fs_is_dir(cwd));
  EXPECT_FALSE(fs_is_dir(file));
  EXPECT_FALSE(fs_is_dir("ffs_is_dir_not-exist-dir"));
}


TEST_F(TestOnDisk, IsFile){
EXPECT_TRUE(fs_is_file(file));
EXPECT_FALSE(fs_is_exe(file));
EXPECT_FALSE(fs_is_file("ffs_is_file_not-exist-file"));
EXPECT_FALSE(fs_is_file(""));
EXPECT_FALSE(fs_is_file("."));
EXPECT_FALSE(fs_is_file(cwd));
}

TEST_F(TestOnDisk, IsReadable)
{
EXPECT_TRUE(fs_is_readable("."));
EXPECT_TRUE(fs_is_readable(file));
EXPECT_TRUE(fs_is_readable(cwd));

if(fs_is_windows()){
  EXPECT_TRUE(fs_is_readable(sys_drive));
}

if(fs_win32_long_paths_enabled()){
  EXPECT_TRUE(fs_is_readable(R"(\\?\)" + sys_drive + "\\"));
}

EXPECT_TRUE(fs_is_readable("/"));

EXPECT_TRUE(fs_is_readable(nonnull_dir));

}

TEST_F(TestOnDisk, IsWritable)
{
EXPECT_TRUE(fs_is_writable(file));
EXPECT_TRUE(fs_is_writable(cwd));

if(fs_win32_long_paths_enabled()){
  std::string s = fs_as_windows(R"(\\?\)" + fs_canonical(file));
  EXPECT_TRUE(fs_is_writable(s)) << s;
}

EXPECT_TRUE(fs_is_writable(nonnull_dir));

}


TEST_F(TestOnDisk, IsOther){
  EXPECT_FALSE(fs_is_other(""));
  EXPECT_FALSE(fs_is_other("."));
  EXPECT_FALSE(fs_is_other(file));
  EXPECT_FALSE(fs_is_other(cwd));
  EXPECT_FALSE(fs_is_other("ffs_is_other_not-exist-file"));
}


TEST_F(TestOnDisk, StatMode){
  EXPECT_NE(fs_st_mode(file), 0);
  EXPECT_NE(fs_st_mode(cwd), 0);
  EXPECT_EQ(fs_st_mode("ffs_stat_mode_not-exist-file"), 0);
  EXPECT_EQ(fs_st_mode(""), 0);

  EXPECT_NE(fs_st_mode(nonnull_dir), 0);
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

EXPECT_EQ(fs_realpath(nonnull_dir), expected) << "problem with non null-terminated path " << nonnull_dir;
}


TEST_F(TestOnDisk, GetModTime){
EXPECT_GT(fs_get_modtime(cwd), 0);

EXPECT_GT(fs_get_modtime(nonnull_dir), 0) << "problem with non null-terminated path " << nonnull_dir;
}


TEST_F(TestOnDisk, Touch)
{

EXPECT_TRUE(fs_touch(file));

auto t0 = fs_get_modtime(file);
EXPECT_GT(t0, 0);

EXPECT_TRUE(fs_set_modtime(file));

EXPECT_GE(fs_get_modtime(file), t0);

EXPECT_FALSE(fs_set_modtime("not-exist-file"));

ASSERT_TRUE(fs_touch(nonnull_file));
EXPECT_TRUE(fs_is_file(nonnull_file));
}

TEST_F(TestOnDisk, FilesystemType)
{
std::string t = fs_filesystem_type(sys_drive);

if (t.empty()){
  GTEST_SKIP() << "Unknown filesystem type, see type ID in stderr to update fs_get_type()";
}

t = fs_filesystem_type(nonnull_sys_drive);
EXPECT_FALSE(t.empty()) << "problem with non null-terminated path " << nonnull_sys_drive;

}

TEST_F(TestOnDisk, Removable)
{
EXPECT_FALSE(fs_is_removable(sys_drive)) << "we assume that a CI system's system drive would not be removable";
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
