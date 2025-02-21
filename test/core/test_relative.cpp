#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestRelative, Relative){

EXPECT_EQ(fs_relative_to("", ""), ".");
EXPECT_EQ(fs_relative_to("Hello", "Hello"), ".");
EXPECT_EQ(fs_relative_to("Hello", "Hello/"), ".");
EXPECT_EQ(fs_relative_to("a/b", "a/b"), ".");
EXPECT_EQ(fs_relative_to("a///b", "a/b"), ".");
EXPECT_EQ(fs_relative_to("a/b", "a///b"), ".");
EXPECT_EQ(fs_relative_to("a/b", "a/b/"), ".");
EXPECT_EQ(fs_relative_to("a/b/", "a/b"), ".");
EXPECT_EQ(fs_relative_to("a/b", "a"), "..");
EXPECT_EQ(fs_relative_to("a/b", "a/"), "..");
EXPECT_EQ(fs_relative_to("a", "a/b/"), "b");
EXPECT_EQ(fs_relative_to("a", "a/b/."), "b");
EXPECT_EQ(fs_relative_to("a", "a/b/.."), ".");
EXPECT_EQ(fs_relative_to("a/b/c/d", "a/b"), "../..");
EXPECT_EQ(fs_relative_to("a/b/c/d", "a/b/"), "../..");
EXPECT_EQ(fs_relative_to("./a/b", "./a/c"), "../c");

if(fs_is_windows()){
  std::string c = fs_getenv("SYSTEMDRIVE");

  if (c.length() == 2){
      EXPECT_EQ(fs_relative_to(c+"/", c+"/a/b"), "a/b");
      EXPECT_EQ(fs_relative_to(c+"/a/b", c+"/"), "../..");
      EXPECT_EQ(fs_relative_to(c+"/a/b", c+"/a/b"), ".");
      EXPECT_EQ(fs_relative_to(c+"/a/b", c+"/a"), "..");
      // {c+"/a", "b", ""} //  ambiguous with Clang/Flang ARM MinGW <filesystem>
  } else {
    std::cerr << "Warning: SYSTEMDRIVE not set, skipping tests\n";
  }
// NOTE: on Windows, if a path is real, finalPath is used, which makes drive letters upper case.

} else {
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

}
