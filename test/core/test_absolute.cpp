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

  ctx.cwd = ffilesystem::get_cwd();
  expect(!ctx.cwd.empty() >> fatal);

  if (ffilesystem::is_windows) {
    ctx.base = "j:\\foo";
    if (const auto d = ffilesystem::getenv("SystemDrive")) {
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

    expect(eq(ffilesystem::absolute(""), ctx.cwd));
    expect(eq(ffilesystem::absolute("", ""), ctx.cwd));
    expect(eq(ffilesystem::absolute("rel", ctx.base), ctx.ref));
    expect(eq(ffilesystem::absolute(ctx.cwd + ffilesystem::file_separator + "rel"), ctx.cwd + ffilesystem::file_separator + "rel"));

    std::string r = ctx.cwd + ffilesystem::file_separator;
    if (!ffilesystem::is_windows || (fs_is_mingw() && fs_is_clang() && ffilesystem::backend == "<filesystem>")) r += dot + ffilesystem::file_separator;

    r += "rel";

    expect(eq(ffilesystem::absolute(dot + ffilesystem::file_separator + "rel"), r));

    // absolute("./rel") may be "/cwd/./rel" (our method, and most <filesystem> except Windows)
    //                     or "/cwd/rel" (Windows <filesystem>)
    // using for base "." or ".." and similar has similar ambiguity for testing.

    // relative path, empty base
    expect(eq(ffilesystem::absolute("rel", ""), ctx.cwd + ffilesystem::file_separator + "rel"));

    // empty path, relative base
    expect(eq(ffilesystem::absolute("", "rel"), ctx.cwd + ffilesystem::file_separator + "rel"));

    expect(eq(ffilesystem::absolute("日本語"), ctx.cwd + ffilesystem::file_separator + "日本語"));
    expect(eq(ffilesystem::absolute("have space"), ctx.cwd + ffilesystem::file_separator + "have space"));

    if (ffilesystem::is_windows) {
      expect(!ctx.sys_drive.empty());
      expect(eq(ffilesystem::absolute(ctx.sys_drive + ffilesystem::file_separator), ctx.sys_drive + ffilesystem::file_separator));

      const std::string cwd_root = ffilesystem::root(ctx.cwd);
      const std::string cwd_root_name = ffilesystem::root_name(ctx.cwd);
      expect(!cwd_root.empty() >> fatal);
      expect(!cwd_root_name.empty() >> fatal);

      std::string drive_relative = cwd_root_name + "rel";
      expect(!ffilesystem::is_absolute(drive_relative));
      std::string expected_drive_relative = ctx.cwd;
      if (expected_drive_relative.back() != '/' && expected_drive_relative.back() != '\\')
        expected_drive_relative.push_back(ffilesystem::file_separator);
      expected_drive_relative += "rel";
      expect(eq(ffilesystem::absolute(drive_relative), expected_drive_relative));

      std::string root_directory_relative = ffilesystem::root_directory(ctx.cwd) + "rel";
      expect(!ffilesystem::is_absolute(root_directory_relative));
      expect(eq(ffilesystem::absolute(root_directory_relative), cwd_root + "rel"));

      // NOTE: no, as MSYS interprets "/" totally differently depending on backend and vs MSVC
      // std::string s = ffilesystem::absolute("/");
      // ffilesystem::as_posix(s);
      // expect(eq(ffilesystem::drop_slash(s), cwd));

      if (fs_is_msvc() && ffilesystem::backend == "<filesystem>" && fs_win32_long_paths_enabled()) {
        expect(eq(ffilesystem::absolute(R"(\\?\X:\anybody)"), std::string{R"(\\?\X:\anybody)"}));
        expect(eq(ffilesystem::absolute(R"(\\?\UNC\server\share)"), std::string{R"(\\?\UNC\server\share)"}));
      }
    } else {
      expect(eq(ffilesystem::absolute("/"), std::string{"/"}));
    }
  };

  "is_absolute"_test = [] {
    const auto ctx = make_ctx();

    expect(!ffilesystem::is_absolute(""));
    expect(!ffilesystem::is_absolute("日本語"));
    expect(!ffilesystem::is_absolute("some space here"));

    if (ffilesystem::is_windows) {
      expect(!ctx.sys_drive.empty());
      expect(ffilesystem::is_absolute(ctx.sys_drive + "/"));

      expect(ffilesystem::is_absolute("J:/"));
      expect(ffilesystem::is_absolute("j:/"));
      expect(!ffilesystem::is_absolute("j:"));
      expect(!ffilesystem::is_absolute("/"));
      expect(!ffilesystem::is_absolute("/日本語"));

      if (fs_is_msvc() && ffilesystem::backend == "<filesystem>" && fs_win32_long_paths_enabled()) {
        expect(ffilesystem::is_absolute(R"(\\?\)"));
        expect(ffilesystem::is_absolute(R"(\\.\)"));

        expect(ffilesystem::is_absolute(R"(\\?\C:\)"));
        expect(ffilesystem::is_absolute(R"(\\.\C:\)"));
        expect(ffilesystem::is_absolute(R"(\\?\UNC\server\share)"));
        expect(ffilesystem::is_absolute(R"(\\?\UNC\server\share\日本語)"));
        expect(ffilesystem::is_absolute(R"(\\server\share\some space here)"));
        expect(ffilesystem::is_absolute(R"(\\?\C:\some space here)"));

        const std::string unc_prefixed = R"(\\server\share\must-not-be-read)";
        const std::string_view truncated_unc(unc_prefixed.data(), 2);
        expect(eq(truncated_unc, std::string_view{R"(\\)"}) >> fatal);
        expect(!ffilesystem::is_absolute(truncated_unc));
      }
    } else {
      expect(ffilesystem::is_absolute("/"));
      expect(ffilesystem::is_absolute("/日本語"));
      expect(!ffilesystem::is_absolute("j:/"));
    }
  };
}
