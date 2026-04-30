#include <string>
#include <string_view>

#include "ffilesystem.h"

#include <boost/ut.hpp>


namespace {
  using namespace boost::ut;

struct canonical_ctx {
  std::string cwd;
  std::string cwdp;
};

auto one_of(const std::string& actual, std::string_view expected1, std::string_view expected2) {
  return actual == expected1 || actual == expected2;
}


auto make_ctx() {
  canonical_ctx ctx{};

  ctx.cwd = fs_get_cwd();
  expect(!ctx.cwd.empty() >> fatal);
  ctx.cwd = fs_realpath(ctx.cwd);
  // realpath for symlinks (macOS), Dev Drive or short name on CI (Windows), network drives, etc.
  ctx.cwd = fs_drop_slash(fs_as_posix(ctx.cwd));
  expect(!ctx.cwd.empty() >> fatal);

  if (ctx.cwd.empty() || ctx.cwd == fs_root(fs_absolute("/"))) {
    return std::optional<canonical_ctx>{};
  }

  expect(fs_set_cwd(ctx.cwd) >> fatal);
  ctx.cwdp = fs_parent(ctx.cwd);
  expect(!ctx.cwdp.empty() && ctx.cwdp != ctx.cwd >> fatal);

  return std::optional<canonical_ctx>{ctx};
}

auto system_drive() {
  const auto value = fs_getenv("SystemDrive");
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
    expect(static_cast<bool>(ctx));
    if (!ctx) {
      return;
    }

    std::string r = fs_canonical("..", true);
    expect(!r.empty() >> fatal);
    expect(eq(fs_as_posix(r), ctx->cwdp));

    r = fs_canonical("..", false);
    expect(!r.empty() >> fatal);
    expect(eq(fs_as_posix(r), ctx->cwdp));

    if (fs_is_windows()) {
      const auto sys_drive = system_drive();
      expect(static_cast<bool>(sys_drive) >> fatal);
      if (!sys_drive) {
        return;
      }

      expect(one_of(fs_resolve(*sys_drive + "/", true), *sys_drive + "\\", *sys_drive + "/"));
      expect(one_of(fs_resolve(*sys_drive + "/", false), *sys_drive + "\\", *sys_drive + "/"));

      if (fs_backend() != "<filesystem>") {
        expect(one_of(fs_resolve(R"(\\?\)" + *sys_drive + "\\", true),
                      R"(\\?\)" + *sys_drive + "\\",
                      R"(\\?\)" + *sys_drive + "/"));
      }
    }
  };

  "resolve_parent_dir"_test = [] {
    const auto ctx = make_ctx();
    expect(static_cast<bool>(ctx));
    if (!ctx) {
      return;
    }

    std::string r = fs_resolve("..", true);
    expect(!r.empty() >> fatal);
    expect(eq(fs_as_posix(r), ctx->cwdp));

    r = fs_resolve("..", false);
    expect(!r.empty() >> fatal);
    expect(eq(fs_as_posix(r), ctx->cwdp));

    if (fs_is_windows()) {
      const auto sys_drive = system_drive();
      expect(static_cast<bool>(sys_drive) >> fatal);
      if (!sys_drive) {
        return;
      }

      expect(one_of(fs_canonical(*sys_drive + "/", true), *sys_drive + "\\", *sys_drive + "/"));
      expect(one_of(fs_canonical(*sys_drive + "/", false), *sys_drive + "\\", *sys_drive + "/"));
      expect(one_of(fs_canonical("M:/", false), "M:\\", "M:/"));

      if (fs_backend() != "<filesystem>") {
        expect(one_of(fs_canonical(R"(\\?\)" + *sys_drive + "\\", true),
                      R"(\\?\)" + *sys_drive + "\\",
                      R"(\\?\)" + *sys_drive + "/"));
      }
    }
  };

  "canonical_parent_rel"_test = [] {
    const auto ctx = make_ctx();
    expect(static_cast<bool>(ctx));
    if (!ctx) {
      return;
    }

    expect(one_of(fs_canonical("../not-exist", false), "../not-exist", ctx->cwdp + "/not-exist"));
    expect(one_of(fs_canonical("./not-exist", false), "not-exist", ctx->cwd + "/not-exist"));
    expect(one_of(fs_canonical("a/b/../c", false), "a/c", ctx->cwd + "/a/c"));
  };

  "resolve_parent_rel"_test = [] {
    const auto ctx = make_ctx();
    expect(static_cast<bool>(ctx));
    if (!ctx) {
      return;
    }

    expect(eq(fs_resolve("../not-exist", false), ctx->cwdp + "/not-exist"));
    expect(eq(fs_resolve("./not-exist", false), ctx->cwd + "/not-exist"));
    expect(eq(fs_resolve("a/b/../c", false), ctx->cwd + "/a/c"));
  };

  "relative_file"_test = [] {
    const auto ctx = make_ctx();
    expect(static_cast<bool>(ctx));
    if (!ctx || fs_is_cygwin()) {
      return;
    }

    const std::string name = "ffs_not-exist_cpp.txt";
    std::string h = fs_canonical("../" + name, false);
    expect(!h.empty());
    expect(h.length() > name.length());
    expect(h.ends_with(name));

    const std::string r = "日本語";
    h = fs_canonical(r, false);
    expect(h.ends_with(r));
  };

  "realpath"_test = [] {
    const auto ctx = make_ctx();
    expect(static_cast<bool>(ctx));
    if (!ctx) {
      return;
    }

    const std::string r = fs_realpath(".");
    expect(!r.empty() >> fatal);
    expect(eq(fs_as_posix(r), ctx->cwd));
  };
}
