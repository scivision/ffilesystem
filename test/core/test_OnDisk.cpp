#include "ffilesystem.h"
#include "internal.h"

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <boost/ut.hpp>

namespace {

struct ondisk_ctx {
  std::string self;
  std::string self_name;
  std::string cwd;
  std::string sys_drive;
  std::string in_dir;
  std::string in_sys_dir;
  std::string_view nonnull_dir;
  std::string_view nonnull_sys_drive;
};

void setup(ondisk_ctx& ctx, std::string_view arg0) {
  using namespace boost::ut;

  ctx.cwd = ffilesystem::get_cwd();
  expect(!ctx.cwd.empty() >> fatal);

  if (ffilesystem::is_windows) {
    auto d = ffilesystem::getenv("SystemDrive");
    expect(d.has_value() >> fatal) << "Failed to get SystemDrive";
    ctx.sys_drive = d.value();
  } else {
    ctx.sys_drive = "/";
  }

  ctx.self = std::string{arg0};
  ctx.self_name = ffilesystem::file_name(ctx.self);

  ctx.in_dir = "./invalid-memory-trailing-non-null-terminated-string_view";
  ctx.nonnull_dir = std::string_view(ctx.in_dir.data(), 2);
  expect(ctx.nonnull_dir.back() != '\0' >> fatal) << "nonnull_dir should not be null-terminated\n";

  ctx.in_sys_dir = ctx.sys_drive + "/invalid-memory-trailing-non-null-terminated-string_view";
  ctx.nonnull_sys_drive = std::string_view(ctx.sys_drive.data(), ctx.sys_drive.size());
  expect(ctx.nonnull_sys_drive.back() != '\0' >> fatal) << "nonnull_sys_drive should not be null-terminated\n";
}

} // namespace

int main(int argc, char** argv) {
  using namespace boost::ut;

  if (!ffilesystem::is_writable(".")) {
    skip / "exists"_test = [] {};
    skip / "is_dir"_test = [] {};
    skip / "is_file"_test = [] {};
    skip / "is_readable"_test = [] {};
    skip / "is_writable"_test = [] {};
    skip / "is_other"_test = [] {};
    skip / "stat_mode"_test = [] {};
    skip / "realpath"_test = [] {};
    skip / "get_mod_time"_test = [] {};
    skip / "touch"_test = [] {};
    skip / "filesystem_type"_test = [] {};
    skip / "removable"_test = [] {};
  } else {

  "exists"_test = [argv] {
    ondisk_ctx ctx;
    setup(ctx, argv[0]);

    "exists cases"_test = [](auto const& test_case) {
      expect(eq(ffilesystem::exists(test_case.first), test_case.second)) << test_case.first;
    } | std::vector<std::pair<std::string_view, bool>>{
            {".", true},
            {"..", true},
            {"/", true},
            {ctx.self, true},
            {ctx.self_name, true},
            {ctx.cwd, true},
            {"fffilesystem::exists_not-exist-file", false},
            {"", false},
            {ctx.nonnull_dir, true},
        };
  };

  "is_dir"_test = [argv] {
    ondisk_ctx ctx;
    setup(ctx, argv[0]);

    "is_dir cases"_test = [](auto const& test_case) {
      expect(eq(ffilesystem::is_directory(test_case.first), test_case.second)) << test_case.first;
    } | std::vector<std::pair<std::string_view, bool>>{
            {"", false},
            {".", true},
            {ctx.cwd, true},
            {ctx.self, false},
            {"fffilesystem::is_dir_not-exist-dir", false},
        };
  };

  "is_file"_test = [argv] {
    ondisk_ctx ctx;
    setup(ctx, argv[0]);

    "is_file cases"_test = [](auto const& test_case) {
      expect(eq(ffilesystem::is_file(test_case.first), test_case.second)) << test_case.first;
    } | std::vector<std::pair<std::string_view, bool>>{
            {ctx.self, true},
            {"fffilesystem::is_file_not-exist-file", false},
            {"", false},
            {".", false},
            {ctx.cwd, false},
        };
  };

  "is_readable"_test = [argv] {
    ondisk_ctx ctx;
    setup(ctx, argv[0]);

    "is_readable cases"_test = [](auto const& path) {
      expect(ffilesystem::is_readable(path)) << path;
    } | std::vector<std::string_view>{
            ".", ctx.self, ctx.cwd, "/", ctx.nonnull_dir,
        };

    if (ffilesystem::is_windows) {
      expect(ffilesystem::is_readable(ctx.sys_drive));

      if (fs_win32_long_paths_enabled()) {
        expect(ffilesystem::is_readable(R"(\\?\)" + ctx.sys_drive + "\\"));
      }
    }
  };

  "is_writable"_test = [argv] {
    ondisk_ctx ctx;
    setup(ctx, argv[0]);

    if (!fs_is_cygwin() && !fs_is_bsd()) {
      expect(ffilesystem::is_writable(ctx.self)) << "not every platform can detect writability of the current executable, but most can";
    }

    expect(ffilesystem::is_writable(ctx.cwd));

    if (ffilesystem::is_windows) {
      if (fs_win32_long_paths_enabled()) {
        std::string s = R"(\\?\)" + ctx.self;
        expect(ffilesystem::is_writable(s)) << s;
      }
    } else if (!ffilesystem::is_admin() && !fs_is_cygwin()) {
      expect(!ffilesystem::is_writable("/"));
    }

    expect(ffilesystem::is_writable(ctx.nonnull_dir));
  };

  "is_other"_test = [argv] {
    ondisk_ctx ctx;
    setup(ctx, argv[0]);

    "is_other cases"_test = [](auto const& path) {
      expect(!ffilesystem::is_other(path)) << path;
    } | std::vector<std::string_view>{
            "", ".", ctx.self, ctx.cwd, "ffilesystem_is_other_not-exist-file",
        };
  };

  "stat_mode"_test = [argv] {
    ondisk_ctx ctx;
    setup(ctx, argv[0]);

    "stat_mode cases"_test = [](auto const& test_case) {
      expect(eq(fs_st_mode(test_case.first) != 0, test_case.second)) << test_case.first;
    } | std::vector<std::pair<std::string_view, bool>>{
            {ctx.self, true},
            {ctx.cwd, true},
            {"ffs_stat_mode_not-exist-file", false},
            {"", false},
            {ctx.nonnull_dir, true},
        };
  };

  "realpath"_test = [argv] {
    ondisk_ctx ctx;
    setup(ctx, argv[0]);

    std::string expected = ffilesystem::realpath(ctx.cwd);
    expect(!expected.empty() >> fatal);

    std::string r = ffilesystem::realpath(".");
    expect(!r.empty() >> fatal);
    expect(eq(r, expected)) << r << " vs " << expected;

    expect(ffilesystem::realpath("not-exist-realpath/b/c").empty());

    r = ffilesystem::realpath("..");
    expect(!r.empty() >> fatal);
    expect(eq(r, ffilesystem::parent(expected))) << r;

    expect(eq(ffilesystem::realpath(ctx.nonnull_dir), expected))
        << "problem with non null-terminated path " << ctx.nonnull_dir;
  };

  "get_mod_time"_test = [argv] {
    ondisk_ctx ctx;
    setup(ctx, argv[0]);

    "get_mod_time cases"_test = [](auto const& path) {
      expect(ffilesystem::get_modtime(path) > 0) << path;
    } | std::vector<std::string_view>{ctx.cwd, ctx.nonnull_dir};
  };

  "touch"_test = [argv] {
    ondisk_ctx ctx;
    setup(ctx, argv[0]);

    std::string_view file = "Ffilesystem_touch_test_file";
    expect(ffilesystem::touch(file));

    auto t0 = ffilesystem::get_modtime(file);
    expect(t0 > 0);

    expect(ffilesystem::set_modtime(file));
    expect(ffilesystem::get_modtime(file) >= t0);
    expect(!ffilesystem::set_modtime("not-exist-file"));

    std::string in_file = ctx.self + "-invalid-memory-trailing-non-null-terminated-string_view";
    std::string_view nonnull_file = std::string_view(in_file.data(), ctx.self.size());
    expect(nonnull_file.back() != '\0' >> fatal) << "nonnull_file should not be null-terminated\n";

    expect(ffilesystem::touch(nonnull_file) >> fatal);
    expect(ffilesystem::is_file(nonnull_file));

   ffilesystem::remove(file);
  };

#if !defined(FFS_EXTRA_ENABLED)
  skip /
#endif
  "filesystem_type"_test = [argv] {
    ondisk_ctx ctx;
    setup(ctx, argv[0]);

    std::string t = ffilesystem::filesystem_type(ctx.sys_drive);
    expect(!t.empty() >> fatal) << "Failed to get filesystem type for " << ctx.sys_drive;

    t = ffilesystem::filesystem_type(ctx.nonnull_sys_drive);
    expect(!t.empty()) << "problem with non null-terminated path " << ctx.nonnull_sys_drive;
  };

  "removable"_test = [argv] {
    ondisk_ctx ctx;
    setup(ctx, argv[0]);

    expect(!ffilesystem::is_removable(ctx.sys_drive))
        << "we assume that a CI system's system drive would not be removable";
  };
}

#if !defined(_WIN32)
  skip /
#endif
  "short_long"_test = [] {
    // the path must exist
    auto e = ffilesystem::getenv("PROGRAMFILES");
    expect(e.has_value() >> fatal) << "Failed to get PROGRAMFILES environment variable";
    std::string long_path = e.value();
    expect(!long_path.empty() >> fatal);

    std::string short_path = ffilesystem::shortname(long_path);

    expect(neq(short_path, long_path));
    expect(eq(ffilesystem::longname(short_path), long_path));
  };
}
