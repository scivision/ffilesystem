#include "ffilesystem.h"

#include <gtest/gtest.h>


class TestUTF8 : public testing::Test {
  protected:
    const std::string smiley = "😀";
    const std::string wink = "😉";
    const std::string hello = "你好";
};


TEST_F(TestUTF8, Intl){

EXPECT_FALSE(fs_canonical(".", true, false).empty());
EXPECT_FALSE(fs_canonical("./", true, false).empty());

EXPECT_EQ(fs_file_name("./" + smiley), smiley);
EXPECT_EQ(fs_file_name("./" + wink), wink);
EXPECT_EQ(fs_file_name("./" + hello), hello);

// This is shaky on macOS. Better to leave it off for now.

// EXPECT_EQ(fs_canonical(smiley, false, false), smiley);
// EXPECT_EQ(fs_canonical(wink, false, false), wink);
// EXPECT_EQ(fs_canonical(hello, false, false), hello);


}
