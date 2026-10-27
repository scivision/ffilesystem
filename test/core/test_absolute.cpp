#include "ffilesystem.h"

#include <string>
#include <string_view>

#include <boost/ut.hpp>
namespace {
  using namespace boost::ut;

struct absolute_ctx {
  std::string base;
  std::string ref;
  std::string cwd;
  std::string sys_drive;
};

auto make_ctx() {
  absolute_ctx ctx{};

  ctx.cwd = fs_get_cwd();
  expect(!ctx.cwd.empty() >> fatal);

  if (fs_is_windows()) {
    ctx.base = "j:\\foo";
    if (const auto d = fs_getenv("SystemDrive")) {
      expect(d.has_value() >> fatal);
      ctx.sys_drive = d.value();
    }
  } else {
    ctx.base = "/foo";
  }

  ctx.ref = ctx.base + ffilesystem::file_separator + "rel";

  return ctx;
}

} // namespace

int main() {
  using namespace boost::ut;

  "absolute"_test = [] {
    const auto ctx = make_ctx();

    const std::string dot = ".";

    expect(eq(fs_absolute(""), ctx.cwd));
    expect(eq(fs_absolute("", ""), ctx.cwd));
    expect(eq(fs_absolute("rel", ctx.base), ctx.ref));
    expect(eq(fs_absolute(ctx.cwd + ffilesystem::file_separator + "rel"), ctx.cwd + ffilesystem::file_separator + "rel"));

    std::string r = ctx.cwd + ffilesystem::file_separator;
    if (!fs_is_windows() || (fs_is_mingw() && fs_is_clang() && fs_backend() == "<filesystem>")) r += dot + ffilesystem::file_separator;

    r += "rel";

    expect(eq(fs_absolute(dot + ffilesystem::file_separator + "rel"), r));

    // absolute("./rel") may be "/cwd/./rel" (our method, and most <filesystem> except Windows)
    //                     or "/cwd/rel" (Windows <filesystem>)
    // using for base "." or ".." and similar has similar ambiguity for testing.

    // relative path, empty base
    expect(eq(fs_absolute("rel", ""), ctx.cwd + ffilesystem::file_separator + "rel"));

    // empty path, relative base
    expect(eq(fs_absolute("", "rel"), ctx.cwd + ffilesystem::file_separator + "rel"));

    expect(eq(fs_absolute("日本語"), ctx.cwd + ffilesystem::file_separator + "日本語"));
    expect(eq(fs_absolute("have space"), ctx.cwd + ffilesystem::file_separator + "have space"));

    if (fs_is_windows()) {
      expect(!ctx.sys_drive.empty());
      expect(eq(fs_absolute(ctx.sys_drive + ffilesystem::file_separator), ctx.sys_drive + ffilesystem::file_separator));

      const std::string cwd_root = fs_root(ctx.cwd);
      const std::string cwd_root_name = fs_root_name(ctx.cwd);
      expect(!cwd_root.empty() >> fatal);
      expect(!cwd_root_name.empty() >> fatal);

      std::string drive_relative = cwd_root_name + "rel";
      expect(!fs_is_absolute(drive_relative));
      std::string expected_drive_relative = ctx.cwd;
      if (expected_drive_relative.back() != '/' && expected_drive_relative.back() != '\\')
        expected_drive_relative.push_back(ffilesystem::file_separator);
      expected_drive_relative += "rel";
      expect(eq(fs_absolute(drive_relative), expected_drive_relative));

      std::string root_directory_relative = fs_root_directory(ctx.cwd) + "rel";
      expect(!fs_is_absolute(root_directory_relative));
      expect(eq(fs_absolute(root_directory_relative), cwd_root + "rel"));

      // NOTE: no, as MSYS interprets "/" totally differently depending on backend and vs MSVC
      // std::string s = fs_absolute("/");
      // fs_as_posix(s);
      // expect(eq(fs_drop_slash(s), cwd));

      if (fs_is_msvc() && fs_backend() == "<filesystem>" && fs_win32_long_paths_enabled()) {
        expect(eq(fs_absolute(R"(\\?\X:\anybody)"), std::string{R"(\\?\X:\anybody)"}));
        expect(eq(fs_absolute(R"(\\?\UNC\server\share)"), std::string{R"(\\?\UNC\server\share)"}));
      }
    } else {
      expect(eq(fs_absolute("/"), std::string{"/"}));
    }
  };

  "is_absolute"_test = [] {
    const auto ctx = make_ctx();

    expect(!fs_is_absolute(""));
    expect(!fs_is_absolute("日本語"));
    expect(!fs_is_absolute("some space here"));

    if (fs_is_windows()) {
      expect(!ctx.sys_drive.empty());
      expect(fs_is_absolute(ctx.sys_drive + "/"));

      expect(fs_is_absolute("J:/"));
      expect(fs_is_absolute("j:/"));
      expect(!fs_is_absolute("j:"));
      expect(!fs_is_absolute("/"));
      expect(!fs_is_absolute("/日本語"));

      if (fs_is_msvc() && fs_backend() == "<filesystem>" && fs_win32_long_paths_enabled()) {
        expect(fs_is_absolute(R"(\\?\)"));
        expect(fs_is_absolute(R"(\\.\)"));

        expect(fs_is_absolute(R"(\\?\C:\)"));
        expect(fs_is_absolute(R"(\\.\C:\)"));
        expect(fs_is_absolute(R"(\\?\UNC\server\share)"));
        expect(fs_is_absolute(R"(\\?\UNC\server\share\日本語)"));
        expect(fs_is_absolute(R"(\\server\share\some space here)"));
        expect(fs_is_absolute(R"(\\?\C:\some space here)"));

        const std::string unc_prefixed = R"(\\server\share\must-not-be-read)";
        const std::string_view truncated_unc(unc_prefixed.data(), 2);
        expect(eq(truncated_unc, std::string_view{R"(\\)"}) >> fatal);
        expect(!fs_is_absolute(truncated_unc));
      }
    } else {
      expect(fs_is_absolute("/"));
      expect(fs_is_absolute("/日本語"));
      expect(!fs_is_absolute("j:/"));
    }
  };
}
