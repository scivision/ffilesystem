#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestProximate, Proximate){

EXPECT_EQ(fs_proximate_to("", ""), ".");
EXPECT_EQ(fs_proximate_to("Hello", "Hello"), ".");
EXPECT_EQ(fs_proximate_to("Hello", "Hello/"), ".");
EXPECT_EQ(fs_proximate_to("a/b", "a/b"), ".");
EXPECT_EQ(fs_proximate_to("a///b", "a/b"), ".");
EXPECT_EQ(fs_proximate_to("a/b", "a///b"), ".");
EXPECT_EQ(fs_proximate_to("a/b", "a/b/"), ".");
EXPECT_EQ(fs_proximate_to("a/b/", "a/b"), ".");
EXPECT_EQ(fs_proximate_to("a/b", "a"), "..");
EXPECT_EQ(fs_proximate_to("a/b", "a/"), "..");
EXPECT_EQ(fs_proximate_to("a", "a/b/"), "b");
EXPECT_EQ(fs_proximate_to("a", "a/b/."), "b");
EXPECT_EQ(fs_proximate_to("a", "a/b/.."), ".");
EXPECT_EQ(fs_proximate_to("a/b/c/d", "a/b"), "../..");
EXPECT_EQ(fs_proximate_to("a/b/c/d", "a/b/"), "../..");
EXPECT_EQ(fs_proximate_to("./a/b", "./a/c"), "../c");

std::vector<std::tuple<std::string, std::string, std::string>> tos;

if(fs_is_windows()){

  std::string c = fs_getenv("SYSTEMDRIVE");

  if (c.length() == 2){
    EXPECT_EQ(fs_proximate_to(c+"/", c+"/a/b"), "a/b");
    EXPECT_EQ(fs_proximate_to(c+"/a/b", c+"/a/b"), ".");
    EXPECT_EQ(fs_proximate_to(c+"/a/b", c+"/a"), "..");
      // {c+"/a", "b", "b"}  //  ambiguous with Clang/Flang ARM MinGW <filesystem>
  } else {
    std::cerr << "Warning: SYSTEMDRIVE not set, skipping tests\n";
  }
// NOTE: on Windows, if a path is real, finalPath is used, which makes drive letters upper case.
// we didn't use a random drive letter because a removable drive at that letter can make the test fail spuriously.

} else {
EXPECT_EQ(fs_proximate_to("", "a"), "a");
EXPECT_EQ(fs_proximate_to("/", "/"), ".");
EXPECT_EQ(fs_proximate_to("Hello", "Hello"), ".");
EXPECT_EQ(fs_proximate_to("Hello", "Hello/"), ".");
EXPECT_EQ(fs_proximate_to("/dev/null", "/dev/null"), ".");
EXPECT_EQ(fs_proximate_to("a/b", "c"), "../../c");
EXPECT_EQ(fs_proximate_to("c", "a/b"), "../a/b");
EXPECT_EQ(fs_proximate_to("a/b", "a/c"), "../c");
}
// NOTE: use relative non-existing paths, as on macOS AppleClang, the <filesystem> gives incorrect results on non-existing absolute paths,
// Which don't make sense anyway.

}
