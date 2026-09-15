#include "ffilesystem.h"

#include <string>
#include <vector>

#include "win32_path.h"

#include <boost/ut.hpp>

int main() {
using namespace boost::ut;

  "ToWide"_test = [] {
    std::string_view s{"hello"};
    std::wstring w = fs_win32_to_wide(s);

    expect(w.size() == s.size());
    expect(w == L"hello");

    // test with null terminator input, that the null terminator is discarded from the output string
    s = "hello\0";
    w = fs_win32_to_wide(s);

    expect(w.back() == L'o');
    expect(s.size() == 5) << "s.size() == " << s.size();
    expect(w.size() == s.size()) << "w.size() == " << w.size();
    expect(w == L"hello");

    // test with std::vector<char> input containing a null terminator
    std::vector<char> vec{'h', 'e', 'l', 'l', 'o', '\0'};
    s = std::string_view(vec.data(), vec.size());
    w = fs_win32_to_wide(s);

    expect(w.back() == L'o');
    expect(s.size() == 6) << "s.size() == " << s.size();
    expect(w.size() == 5) << "w.size() == " << w.size();
    expect(w == L"hello");

    // test with std::string_view having no null terminator
    w = fs_win32_to_wide(s.substr(0, 5));

    expect(w.back() == L'o');
    expect(w.size() == 5) << "w.size() == " << w.size();
    expect(w == L"hello");
  };

  "ToNarrow"_test = [] {
    std::wstring_view w{L"hello"};
    std::string n = fs_win32_to_narrow(w);

    expect(n.size() == 5);
    expect(n.size() == w.size());
    expect(n == "hello");

    // test with null terminator input, that the null terminator is discarded from the output string
    w = L"hello\0";
    n = fs_win32_to_narrow(w);

    expect(n.back() == 'o') << "n.back() == " << n.back();
    expect(w.size() == 5) << "w.size() == " << w.size();
    expect(n.size() == w.size()) << "n.size() == " << n.size();
    expect(n == "hello") << "n == " << n;

    // test with std::vector<wchar_t> input containing a null terminator
    std::vector<wchar_t> wvec{L'h', L'e', L'l', L'l', L'o', L'\0'};
    w = std::wstring_view(wvec.data(), wvec.size());
    n = fs_win32_to_narrow(w);

    expect(n.back() == 'o') << "n.back() == " << n.back();
    expect(w.size() == 6) << "w.size() == " << w.size();
    expect(n.size() == 5) << "n.size() == " << n.size();
    expect(n == "hello") << "n == " << n;

    // test with std::string_view having no null terminator
    n = fs_win32_to_narrow(w.substr(0, 5));

    expect(n.back() == 'o');
    expect(n.size() == 5);
    expect(n == "hello");
  };

  "PathQueries"_test = [] {
    const std::string full = fs_win32_full_name(".");
    const std::string final = fs_win32_final_path(".");

    expect(!full.empty() >> fatal);
    expect(!final.empty() >> fatal);
    expect(fs_exists(full));
    expect(fs_exists(final));
    expect(full.find('\0') == std::string::npos);
    expect(final.find('\0') == std::string::npos);
    expect(full.back() != '\0');
    expect(final.back() != '\0');
  };
}
