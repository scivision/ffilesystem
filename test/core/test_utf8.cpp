#include "ffilesystem.h"

#include <gtest/gtest.h>


TEST(TestUTF8, Intl){

const std::string smiley = "😀";
const std::string wink = "😉";
const std::string hello = "你好";

const std::string e = "";

EXPECT_NE(fs_canonical(".", true, false), e);
EXPECT_NE(fs_canonical("./", true, false), e);

for ( const auto &u : {smiley, wink, hello} ) {
  EXPECT_EQ(fs_file_name("./" + u), u);
  EXPECT_EQ(fs_canonical(u, false, false), u);
}

}
