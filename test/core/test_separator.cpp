#include "ffilesystem.h"

#include <boost/ut.hpp>

int main() {
using namespace boost::ut;

"as_posix"_test = [] {

expect(fs_as_posix("").empty());

if(fs_is_windows()){
expect(eq(fs_as_posix(R"(a\b)"), std::string{"a/b"}));
expect(eq(fs_as_posix(R"(C:\my\path)"), std::string{"C:/my/path"}));
}

};

"path_sep"_test = [] {

if(fs_is_windows()){
  expect(eq(fs_pathsep(), ';'));
} else {
  expect(eq(fs_pathsep(), ':'));
}
};
}
