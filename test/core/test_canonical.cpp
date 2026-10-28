#include <string>
#include <string_view>

#include "ffilesystem.h"
#include "ffilesystem_test.hpp"

#include <boost/ut.hpp>


namespace {
  using namespace boost::ut;

struct canonical_ctx {
  std::string cwd;
  std::string cwdp;
};


auto make_ctx() {
  canonical_ctx ctx{};

  ctx.cwd = ffilesystem::get_cwd();
  expect(!ctx.cwd.empty() >> fatal);
  ctx.cwd = ffilesystem::realpath(ctx.cwd);
  // realpath for symlinks (macOS), Dev Drive or short name on CI (Windows), network drives, etc.
  ffilesystem::as_posix(ctx.cwd);
  ctx.cwd = ffilesystem::drop_slash(ctx.cwd);
  expect(!ctx.cwd.empty() >> fatal);

  if (ctx.cwd.empty() || ctx.cwd == ffilesystem::root(ffilesystem::absolute("/"))) {
    return std::optional<canonical_ctx>{};
  }

  expect(ffilesystem::set_cwd(ctx.cwd) >> fatal);
  ctx.cwdp = ffilesystem::parent(ctx.cwd);
  expect(!ctx.cwdp.empty() && ctx.cwdp != ctx.cwd >> fatal);

  return std::optional<canonical_ctx>{ctx};
}

auto system_drive() {
  const auto value = ffilesystem::getenv("SystemDrive");
  if (!value || value->empty()) {
    return std::optional<std::string>{};
  }

  return std::optional<std::string>{*value};
}

} // namespace

int main() {
  using namespace boost::ut;

  "canonical_parent_dir"_test = [] {
    const auto ctx = make_ctx();
    expect(ctx.has_value() >> fatal);

    std::string r = ffilesystem::canonical("..", true);
    expect(!r.empty() >> fatal);
    ffilesystem::as_posix(r);
    expect(eq(r, ctx->cwdp));

    r = ffilesystem::canonical("..", false);
    expect(!r.empty() >> fatal);
    ffilesystem::as_posix(r);
    expect(eq(r, ctx->cwdp));
  };

#ifndef _WIN32
  skip /
#endif
  "canonical_parent_dir_windows"_test = [] {
    const auto sys_drive = system_drive();
    expect(sys_drive.has_value() >> fatal);

    expect(any_of{*sys_drive + "\\", *sys_drive + "/"} == ffilesystem::resolve(*sys_drive + "/", true));
    expect(any_of{*sys_drive + "\\", *sys_drive + "/"} == ffilesystem::resolve(*sys_drive + "/", false));
  };

  "resolve_parent_dir"_test = [] {
    const auto ctx = make_ctx();
    expect(ctx.has_value() >> fatal);

    std::string r = ffilesystem::resolve("..", true);
    expect(!r.empty() >> fatal);
    ffilesystem::as_posix(r);
    expect(eq(r, ctx->cwdp));

    r = ffilesystem::resolve("..", false);
    expect(!r.empty() >> fatal);
    ffilesystem::as_posix(r);
    expect(eq(r, ctx->cwdp));
};

#ifndef _WIN32
    skip /
#endif
  "resolve_system_drive"_test = [] {
    const auto sys_drive = system_drive();
    expect(sys_drive.has_value() >> fatal);

    expect(any_of{*sys_drive + "\\", *sys_drive + "/"} == ffilesystem::canonical(*sys_drive + "/", true));
    expect(any_of{*sys_drive + "\\", *sys_drive + "/"} == ffilesystem::canonical(*sys_drive + "/", false));
    expect(any_of{"M:\\", "M:/"} == ffilesystem::canonical("M:/", false));

    if (ffilesystem::backend != "<filesystem>") {
      expect(any_of{R"(\\?\)" + *sys_drive + "\\", R"(\\?\)" + *sys_drive + "/"} == ffilesystem::canonical(R"(\\?\)" + *sys_drive + "\\", true));
    }
};

  "canonical_parent_rel"_test = [] {
    const auto ctx = make_ctx();
    expect(ctx.has_value() >> fatal);

    expect(any_of{"../not-exist", ctx->cwdp + "/not-exist"} == ffilesystem::canonical("../not-exist", false));
    expect(any_of{"not-exist", ctx->cwd + "/not-exist"} == ffilesystem::canonical("./not-exist", false));
    expect(any_of{"a/c", ctx->cwd + "/a/c"} == ffilesystem::canonical("a/b/../c", false));
  };

  "resolve_parent_rel"_test = [] {
    const auto ctx = make_ctx();
    expect(ctx.has_value() >> fatal);

    expect(eq(ffilesystem::resolve("../not-exist", false), ctx->cwdp + "/not-exist"));
    expect(eq(ffilesystem::resolve("./not-exist", false), ctx->cwd + "/not-exist"));
    expect(eq(ffilesystem::resolve("a/b/../c", false), ctx->cwd + "/a/c"));
  };

#ifdef __CYGWIN__
  skip /
#endif
  "relative_file"_test = [] {
    const auto ctx = make_ctx();
    expect(ctx.has_value() >> fatal);

    const std::string name = "ffs_not-exist_cpp.txt";
    std::string h = ffilesystem::canonical("../" + name, false);
    expect(!h.empty());
    expect(h.length() > name.length());
    expect(h.ends_with(name));

    const std::string r = "日本語";
    h = ffilesystem::canonical(r, false);
    expect(h.ends_with(r));
  };

  "realpath"_test = [] {
    const auto ctx = make_ctx();
    expect(ctx.has_value() >> fatal);

    std::string r = ffilesystem::realpath(".");
    expect(!r.empty() >> fatal);
    ffilesystem::as_posix(r);
    expect(eq(r, ctx->cwd));
  };
}
