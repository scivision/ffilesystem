#include "ffilesystem.h"

#include <boost/ut.hpp>

namespace {

struct mkdir_ctx {
  std::string dir;
  std::string cwd;
  std::string in_dir;
  std::string_view nonnull_dir;

  ~mkdir_ctx() {
    if (!dir.empty()) {
      fs_remove(dir);
    }
  }
};

auto setup(mkdir_ctx& ctx, std::string_view test_name) -> bool {
  using namespace boost::ut;

  ctx.cwd = fs_get_cwd();
  if (!fs_is_writable(ctx.cwd)) {
    return false;
  }

  ctx.dir = ctx.cwd + "/ffs_test_" + std::string{test_name} + "_dir";

  ctx.in_dir = "./invalid-memory-trailing-non-null-terminated-string_view";
  ctx.nonnull_dir = std::string_view(ctx.in_dir.data(), 2);
  expect(ctx.nonnull_dir.back() != '\0' >> fatal) << "nonnull_dir should not be null-terminated\n";

  return true;
}

} // namespace

int main() {
  using namespace boost::ut;

  "mkdir"_test = [] {
    mkdir_ctx ctx;
    if (!setup(ctx, "mkdir")) {
      return;
    }

    expect(!fs_mkdir(""));

    // Test mkdir with existing directory
    expect(fs_mkdir(ctx.dir) >> fatal);

    // Test mkdir with relative path
    expect(fs_set_cwd(ctx.dir) >> fatal);

    expect(fs_mkdir("test-filesystem-dir/hello") >> fatal);
    expect(fs_is_dir(ctx.dir + "/test-filesystem-dir/hello"));

    expect(fs_mkdir(ctx.nonnull_dir) >> fatal);
    expect(!fs_is_dir(ctx.in_dir));
  };
}
