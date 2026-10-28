#include "ffilesystem.h"

#include <string_view>
#include <utility>
#include <vector>

#include <boost/ut.hpp>

using namespace boost::ut;

int main() {
  "WithSuffix"_test = [](auto const& test_case) {
    expect(eq(ffilesystem::with_suffix(test_case.first.first, test_case.first.second),
              test_case.second));
  } | std::vector<std::pair<std::pair<std::string_view, std::string_view>,
                            std::string_view>>{
          {{"", ".h5"}, ".h5"},
          {{"foo.h5", ""}, "foo"},
          {{".foo.h5", ".txt"}, ".foo.txt"},
          {{".h5", ""}, ".h5"},
          {{".h5", ".h5"}, ".h5.h5"},
          {{"c:/a/hi.nc", ".h5"}, "c:/a/hi.h5"},
          {{"my/file.h5", ".hdf5"}, "my/file.hdf5"},
          {{"a/boo", ".h5"}, "a/boo.h5"},
          {{"boo", ".h5"}, "boo.h5"},
          {{"a/b/c.d/", ".hdf5"}, "a/b/c.d/.hdf5"},
          {{"dir.h5/", ".hdf5"}, "dir.h5/.hdf5"},
          {{"a/b/.h5", ".nc"}, "a/b/.h5.nc"},
      };
}
