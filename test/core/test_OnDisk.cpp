#include "ffilesystem.h"

#include <string>
#include <string_view>

#include <gtest/gtest.h>

class TestOnDisk : public testing::Test {
  protected:
    std::string self, self_name, cwd, sys_drive, in_dir, in_sys_dir;
    std::string_view nonnull_dir, nonnull_sys_drive;

    void SetUp() override {
      auto inst = testing::UnitTest::GetInstance();
      auto info = inst->current_test_info();

      // https://google.github.io/googletest/reference/testing.html#UnitTest::current_test_suite

      cwd = fs_get_cwd();
      ASSERT_FALSE(cwd.empty());

      if(!fs_is_writable(cwd)){
        GTEST_SKIP() << "current directory is not writable" << cwd;
      }

      if (fs_is_windows()) {
        auto d = fs_getenv("SystemDrive");
        ASSERT_TRUE(d.has_value()) << "Failed to get SystemDrive";
        sys_drive = d.value();
      } else {
        sys_drive = "/";
      }

      std::vector<std::string> argvs = ::testing::internal::GetArgvs();
      self = argvs[0];
      self_name = fs_file_name(self);

      in_dir = "./invalid-memory-trailing-non-null-terminated-string_view";
      nonnull_dir = std::string_view(in_dir.data(), 2);
      ASSERT_NE(nonnull_dir.back(), '\0') << "nonnull_dir should not be null-terminated\n";

      in_sys_dir = sys_drive + "/invalid-memory-trailing-non-null-terminated-string_view";
      nonnull_sys_drive = std::string_view(sys_drive.data(), sys_drive.size());
      ASSERT_NE(nonnull_sys_drive.back(), '\0') << "nonnull_sys_drive should not be null-terminated\n";
    }
  };


TEST_F(TestOnDisk, Exists)
{
  for (auto s : {std::string("."), std::string(".."), std::string("/"), self, self_name, cwd})
    EXPECT_TRUE(fs_exists(s)) << "Expected to exist: " << s;

  for (auto s : {"ffs_exists_not-exist-file", ""})
    EXPECT_FALSE(fs_exists(s)) << "Expected to not exist: " << s;

  EXPECT_TRUE(fs_exists(nonnull_dir));
}

TEST_F(TestOnDisk, IsDir)
{
  EXPECT_FALSE(fs_is_dir(""));
  EXPECT_TRUE(fs_is_dir("."));
  EXPECT_TRUE(fs_is_dir(cwd));
  EXPECT_FALSE(fs_is_dir(self));
  EXPECT_FALSE(fs_is_dir("ffs_is_dir_not-exist-dir"));
}


TEST_F(TestOnDisk, IsFile){
EXPECT_TRUE(fs_is_file(self));
EXPECT_FALSE(fs_is_file("ffs_is_file_not-exist-file"));
EXPECT_FALSE(fs_is_file(""));
EXPECT_FALSE(fs_is_file("."));
EXPECT_FALSE(fs_is_file(cwd));
}

TEST_F(TestOnDisk, IsReadable)
{
EXPECT_TRUE(fs_is_readable("."));
EXPECT_TRUE(fs_is_readable(self));
EXPECT_TRUE(fs_is_readable(cwd));

if(fs_is_windows()){
  EXPECT_TRUE(fs_is_readable(sys_drive));

  if(fs_win32_long_paths_enabled())
    EXPECT_TRUE(fs_is_readable(R"(\\?\)" + sys_drive + "\\"));
}

EXPECT_TRUE(fs_is_readable("/"));

EXPECT_TRUE(fs_is_readable(nonnull_dir));

}

TEST_F(TestOnDisk, IsWritable)
{

if (!fs_is_cygwin())
  EXPECT_TRUE(fs_is_writable(self));

EXPECT_TRUE(fs_is_writable(cwd));

if(fs_is_windows()){
  if(fs_win32_long_paths_enabled()){
    std::string s = fs_as_windows(R"(\\?\)" + fs_canonical(self));
    EXPECT_TRUE(fs_is_writable(s)) << s;
  }
} else if (!fs_is_admin() && !fs_is_cygwin()){
  EXPECT_FALSE(fs_is_writable("/"));
}

EXPECT_TRUE(fs_is_writable(nonnull_dir));
}


TEST_F(TestOnDisk, IsOther){
  EXPECT_FALSE(fs_is_other(""));
  EXPECT_FALSE(fs_is_other("."));
  EXPECT_FALSE(fs_is_other(self));
  EXPECT_FALSE(fs_is_other(cwd));
  EXPECT_FALSE(fs_is_other("ffs_is_other_not-exist-file"));
}


TEST_F(TestOnDisk, StatMode){
  EXPECT_NE(fs_st_mode(self), 0);
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
std::string_view file = "ffs_touch_test_file";

EXPECT_TRUE(fs_touch(file));

auto t0 = fs_get_modtime(file);
EXPECT_GT(t0, 0);

EXPECT_TRUE(fs_set_modtime(file));

EXPECT_GE(fs_get_modtime(file), t0);

EXPECT_FALSE(fs_set_modtime("not-exist-file"));

std::string in_file = self + "-invalid-memory-trailing-non-null-terminated-string_view";
std::string_view nonnull_file = std::string_view(in_file.data(), self.size());
ASSERT_NE(nonnull_file.back(), '\0') << "nonnull_file should not be null-terminated\n";

ASSERT_TRUE(fs_touch(nonnull_file));
EXPECT_TRUE(fs_is_file(nonnull_file));

fs_remove(file);
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
