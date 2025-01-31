#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <tuple>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"

#ifdef _MSC_VER
#include <crtdbg.h>
#endif


int main() {
#ifdef _MSC_VER
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

    std::string p;

    int fail = 0;

    std::vector<std::tuple<std::string_view, std::string_view>> test_cases = {
        {"", "."},
        {"/", "/"},
        {"//", "/"},
        {"/////", "/"},
        {".", "."},
        {"./", "."},
        {"./.", "."},
        {"..", ".."},
        {"../", ".."},
        {"a/..", "."},
        {"../..", "../.."},
        {"/a", "/a"},
        {"a", "a"},
        {".a", ".a"},
        {"a.", "a."},
        {"a./", "a."},
        {"..a", "..a"},
        {"a..", "a.."},
        {"a../", "a.."},
        {"a/", "a"},
        {"a//", "a"},
        {"./a", "a"},
        {"./a/", "a"},
        {"./a/.", "a"},
        {"../a", "../a"},
        {"../a/b/..", "../a"},
        {"a/b/", "a/b"},
        {"a/b/.", "a/b"},
        {"a/b/..", "a"},
        {"a/b/../", "a"},
        {"a/b/../c", "a/c"},
        {"a/b/../c/d", "a/c/d"},
        {"a/b/../../c/d", "c/d"},
        {"././a/./b/././c/./.", "a/b/c"},
        {"a/b/../../c/../..", ".."},
        {"a/b/../../../c/../..", "../.."},
        {"a/./b/..", "a"},
        {"a/.///b/../", "a"},
        {"/a/../..", "/"},
        {"/a/../../../../", "/"}
    };
// some tests from https://github.com/gulrak/filesystem/blob/b1982f06c84f08a99fb90bac43c2d03712efe921/test/filesystem_test.cpp#L950

if(fs_is_windows()){
    test_cases.emplace_back(R"(\/\///\/)", "/");
    test_cases.emplace_back(R"(a/b/..\//..///\/../c\\/)", "../c");
    test_cases.emplace_back(R"(..a/b/..\//..///\/../c\\/)", "../c");
    test_cases.emplace_back(R"(..\)" , "..");
    test_cases.emplace_back(R"(c:\)", "c:/");
    test_cases.emplace_back(R"(c:\\)", "c:/");
    test_cases.emplace_back(R"(c:\a/b/../)", "c:/a");
}

    for (const auto& [input, expected] : test_cases) {
        p = fs_normal(input);
        if (p != expected) {
            std::cerr << "FAIL: normal(" << input << ") " << p << " expected " << expected << "\n";
            fail++;
        } else
          std::cout << "OK: normal(" << input << ") " << p << "\n";
    }

    if (fail) {
        std::cerr << "FAILED " << fail << " normal tests\n";
        return EXIT_FAILURE;
    }

    ok_msg("normal C++");

    return EXIT_SUCCESS;
}
