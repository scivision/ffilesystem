#include "ffilesystem.h"

#include <string>
#include <vector>

#include <boost/ut.hpp>

int main() {
using namespace boost::ut;

  "ToWide"_test = [] {
    std::string_view s = "hello";
    std::wstring w = fs_win32_to_wide(s);

    expect(w.size() == s.size());
    expect(w == L"hello");
  };

  "ToNarrow"_test = [] {
    std::wstring_view w = L"hello";
    std::string n = fs_win32_to_narrow(w);

    expect(n.size() == w.size());
    expect(n == "hello");
  };

  if (fs_win32_long_paths_policy_enabled()) {
    "long_path"_test = [] {
      std::string path = fs_get_tempdir();
      if (path.back() != fs_filesep())
        path.push_back(fs_filesep());
      path += "ffilesystem-long-path-" + std::to_string(fs_getpid());

      std::vector<std::string> directories;
      for (int index = 0; index < 32; ++index) {
        path += "/segment-" + std::to_string(index);
        directories.push_back(path);
      }

      expect(path.size() > 260u >> fatal);
      expect(fs_mkdir(path) >> fatal) << path;
      expect(fs_is_dir(path));

      for (auto directory = directories.rbegin(); directory != directories.rend(); ++directory)
        expect(fs_remove(*directory));
    };
  }
}
