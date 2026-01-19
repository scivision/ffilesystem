#include <string>

#include "ffilesystem.h"

#include <gtest/gtest.h>

class TestNoEnv : public testing::Test {
  protected:

    void SetUp() override {
      ASSERT_TRUE(fs_setenv("PATH", ""));
      ASSERT_TRUE(fs_setenv("HOME", ""));
      ASSERT_TRUE(fs_setenv("XDG_CONFIG_HOME", ""));
      ASSERT_TRUE(fs_setenv("TMPDIR", ""));
      if (fs_is_windows()) {
        ASSERT_TRUE(fs_setenv("USERPROFILE", ""));
      }
    }
};


TEST(TestUsername, Username)
{
std::string user = fs_get_username();
EXPECT_FALSE(user.empty());
std::cout << "Username " << user << "\n";
}


TEST(TestEnvironment, Environment)
{

EXPECT_FALSE(fs_get_cwd().empty());

std::string pdir = fs_get_profile_dir();
EXPECT_FALSE(pdir.empty());
std::cout << "Profile directory " << pdir << "\n";

std::string p = fs_get_homedir();
EXPECT_FALSE(p.empty());
std::cout << "Home directory " << p << "\n";
EXPECT_TRUE(fs_is_dir(p));

// NOTE: profiledir does not always (but may) equal homedir, for example when root user.

EXPECT_EQ(fs_expanduser("~"), p);

// --- tempdir
auto t = fs_get_tempdir();
EXPECT_FALSE(t.empty());

std::cout << "Temp directory " << t << "\n";
EXPECT_TRUE(fs_is_dir(t));
}


TEST(TestEnvironment, SetEnv){
std::string k = "FORTtest";
std::string v = "FORTvalue";
fs_setenv(k, v);

auto e = fs_getenv(k);
EXPECT_TRUE(e.has_value()) << "Environment variable " << k << " not set";
EXPECT_EQ(e.value(), v);

fs_setenv(k, ""); // unset
e = fs_getenv(k);
EXPECT_FALSE(e.has_value()) << "Environment variable " << k << " should be unset";
}


TEST_F(TestNoEnv, NoEnvironment)
{

std::string pdir = fs_get_profile_dir();
EXPECT_FALSE(pdir.empty());
std::cout << "Profile directory " << pdir << "\n";

// --- tempdir
auto t = fs_get_tempdir();
EXPECT_FALSE(t.empty());

std::cout << "Temp directory " << t << "\n";
EXPECT_TRUE(fs_is_dir(t));

}


TEST_F(TestNoEnv, Home){
auto h = fs_getenv(fs_is_windows() ? "USERPROFILE" : "HOME");
ASSERT_FALSE(h.has_value()) << "Environment variable HOME or USERPROFILE should not be set in test";

std::string p = fs_get_homedir();
EXPECT_FALSE(p.empty());
std::cout << "Home directory " << p << "\n";
EXPECT_TRUE(fs_is_dir(p));

// NOTE: profiledir does not always (but may) equal homedir, for example when root user.

EXPECT_EQ(fs_expanduser("~"), p);
}
