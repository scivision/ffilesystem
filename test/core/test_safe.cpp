#include "ffilesystem.h"

#include <boost/ut.hpp>

int main() {
using namespace boost::ut;

"safe"_test = [] {

expect(!ffilesystem::is_safe_name("test/re/"));
expect(!ffilesystem::is_safe_name("test/re"));

if(ffilesystem::is_windows)
  expect(!ffilesystem::is_safe_name("hi."));
else
  expect(ffilesystem::is_safe_name("hi."));

expect(!ffilesystem::is_safe_name("hi there"));

expect(!ffilesystem::is_safe_name("日本語"));

};
}
