#include "ffilesystem.h"
#include <string>
#include <utility>
#include <vector>

#include <boost/ut.hpp>

int main() {
    using namespace boost::ut;

    "is_prefix true"_test = [](auto const& test_case) {
        expect(fs_is_prefix(test_case.first, test_case.second));
    } | std::vector<std::pair<std::string, std::string>>{
            {"a/b", "a/b"},
            {"a/b/", "a/b"},
            {"a/b", "a/b/"},
            {"c:/", "c:/a"},
        };

    "is_prefix false"_test = [](auto const& test_case) {
        expect(!fs_is_prefix(test_case.first, test_case.second));
    } | std::vector<std::pair<std::string, std::string>>{
            {"a/b//c", "a/b"},
            {"a/b/c", "a/b/"},
            {"a/b/c", "a"},
            {"/a/b", "a/b"},
            {"a/b", "/a/b"},
            {"a/b", "c"},
            {"b", "a/b"},
            {"c:/a", "c:/"},
        };
}
