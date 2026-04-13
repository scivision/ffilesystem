#include "ffilesystem.h"
#include <gtest/gtest.h>


TEST(TestRelative, RelativeTo)
{

EXPECT_EQ(fs_relative_to("", ""), "");
EXPECT_EQ(fs_relative_to("Hello", "Hello"), ".");
EXPECT_EQ(fs_relative_to("Hello", "Hello/"), ".");
EXPECT_EQ(fs_relative_to("a/b", "a/b"), ".");
EXPECT_EQ(fs_relative_to("a///b", "a/b"), ".");
EXPECT_EQ(fs_relative_to("a/b", "a///b"), ".");
EXPECT_EQ(fs_relative_to("a/b", "a/b/"), ".");
EXPECT_EQ(fs_relative_to("a/b/", "a/b"), ".");
EXPECT_EQ(fs_relative_to("a/b", "a"), "..");
EXPECT_EQ(fs_relative_to("a/b", "a/"), "../");
EXPECT_EQ(fs_relative_to("a", "a/b/"), "b/");
EXPECT_EQ(fs_relative_to("a", "a/b/."), "b/");
EXPECT_EQ(fs_relative_to("a", "a/b/.."), ".");
EXPECT_EQ(fs_relative_to("a/b/c/d", "a/b"), "../..");
EXPECT_EQ(fs_relative_to("a/b/c/d", "a/b/"), "../../");
EXPECT_EQ(fs_relative_to("./a/b", "./a/c"), "../c");

if (fs_is_windows()){
auto e = fs_getenv("SYSTEMDRIVE");
ASSERT_TRUE(e.has_value()) << "Failed to get SYSTEMDRIVE environment variable";
std::string c = e.value();

EXPECT_EQ(fs_relative_to(c+"/", c+"/a/b"), "a/b");
EXPECT_EQ(fs_relative_to(c+"/a/b", c+"/"), "../..");
EXPECT_EQ(fs_relative_to(c+"/a/b", c+"/a/b"), ".");
EXPECT_EQ(fs_relative_to(c+"/a/b", c+"/a"), "..");
// {c+"/a", "b", ""} //  ambiguous with Clang/Flang ARM MinGW <filesystem>

// NOTE: on Windows, if a path is real, finalPath is used, which makes drive letters upper case.

}


EXPECT_EQ(fs_relative_to("", "a"), "a");
EXPECT_EQ(fs_relative_to("/", "/"), ".");
EXPECT_EQ(fs_relative_to("Hello", "Hello"), ".");
EXPECT_EQ(fs_relative_to("Hello", "Hello/"), ".");
EXPECT_EQ(fs_relative_to("/dev/null", "/dev/null"), ".");
EXPECT_EQ(fs_relative_to("a/b", "c/d"), "../../c/d");
EXPECT_EQ(fs_relative_to("c", "a/b"), "../a/b");
EXPECT_EQ(fs_relative_to("a/b", "a/c"), "../c");
// NOTE: use relative non-existing paths, as on macOS AppleClang, the <filesystem> gives incorrect results on non-existing absolute paths,
// Which don't make sense anyway.

}
