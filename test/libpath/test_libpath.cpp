#include "ffilesystem.h"

#include <boost/ut.hpp>

int main() {
using namespace boost::ut;

  "LibPath"_test = [] {
    std::string path = ffilesystem::lib_path();

    expect(!path.empty());
    expect(ffilesystem::exists(path));
    expect(!ffilesystem::is_directory(path));
    expect(ffilesystem::is_file(path));
};
}
