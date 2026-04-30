#include "ffilesystem.h"
#include <string>

#include <boost/ut.hpp>

int main() {
using namespace boost::ut;

"owner_name"_test = [] {
  expect(!fs_get_owner_name(".").empty());
};

"owner_group"_test = [] {
  expect(!fs_get_owner_group(".").empty());
};

// mismatched username and owner can happen

"username"_test = [] {
  expect(!fs_get_username().empty());
};
}
