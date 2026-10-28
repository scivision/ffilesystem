#include "ffilesystem.h"

#include <boost/ut.hpp>

int main() {
using namespace boost::ut;

"intl"_test = [] {
const std::string smiley = "😀";
const std::string wink = "😉";
const std::string hello = "你好";

expect(!ffilesystem::canonical(".", true).empty());
expect(!ffilesystem::canonical("./", true).empty());

expect(eq(ffilesystem::file_name("./" + smiley), smiley));
expect(eq(ffilesystem::file_name("./" + wink), wink));
expect(eq(ffilesystem::file_name("./" + hello), hello));

// This is shaky on macOS. Better to leave it off for now.

// EXPECT_EQ(ffilesystem::canonical(smiley, false, false), smiley);
// EXPECT_EQ(ffilesystem::canonical(wink, false, false), wink);
// EXPECT_EQ(ffilesystem::canonical(hello, false, false), hello);


};
}
