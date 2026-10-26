#include "ffilesystem.h"
#include <string>
#include <utility> // for std::pair
#include <vector>

#include <boost/ut.hpp>


int main() {

using namespace boost::ut;

suite TestExpand = [] {
    std::string const h = fs_get_homedir();
    "Expanduser home directory"_test = [h] {
        expect(!h.empty() >> fatal) << "Home directory should not be empty";
        expect(fs_is_dir(h) >> fatal) << "Home directory should be a directory: " << h;
    };

    "Expanduser"_test = [](auto const& test_case) {
        expect(fs_expanduser(test_case.first) == test_case.second);
    } | std::vector<std::pair<std::string, std::string>>{
            {"", ""},
            {".", "."},
            {"~", h},
            {"~/", h},
            {"~/test", h + "/test"},
            {"~test", "~test"},
            {"test~", "test~"},
            {"test~test", "test~test"},
            {"日本語", "日本語"},
        };
};
}
