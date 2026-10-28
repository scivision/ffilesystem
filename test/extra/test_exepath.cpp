#include "ffilesystem.h"

#include <boost/ut.hpp>


int main() {
  using namespace boost::ut;

  "exe_path"_test = [] {
    std::string path = ffilesystem::exe_path();

    expect(!path.empty());
    expect(ffilesystem::exists(path));
    expect(!ffilesystem::is_directory(path));
    expect(ffilesystem::is_file(path));
    expect(ffilesystem::is_exe(path));
    expect(ffilesystem::is_readable(path));

    if(!fs_is_cygwin())
      expect(ffilesystem::is_executable_binary(path));
  };
}
