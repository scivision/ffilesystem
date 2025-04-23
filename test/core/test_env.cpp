#include <string>

#include "ffilesystem.h"

#include <gtest/gtest.h>

class TestEnvironment : public testing::Test {
    protected:
      std::string cwd;
      void SetUp() override {
        cwd = ::testing::UnitTest::GetInstance()->original_working_dir();
      }
};

TEST_F(TestEnvironment, Environment)
{

EXPECT_TRUE(fs_is_dir(cwd));
EXPECT_EQ(fs_get_cwd(), cwd);

std::string pdir = fs_get_profile_dir();
EXPECT_FALSE(pdir.empty());
std::cout << "Profile directory " << pdir << "\n";

std::string cdir = fs_user_config_dir();
EXPECT_FALSE(cdir.empty());
std::cout << "User config directory " << cdir << "\n";


std::string user = fs_get_username();
EXPECT_FALSE(user.empty());
std::cout << "Username " << user << "\n";

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
EXPECT_TRUE(fs_exists(t));

// --- setenv
std::string k = "FORTtest";
std::string v = "FORTvalue";
fs_setenv(k, v);

EXPECT_EQ(fs_getenv(k), v);

}
