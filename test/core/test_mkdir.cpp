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
     ffilesystem::remove(dir);
    }
  }
};

void setup(mkdir_ctx& ctx, std::string_view test_name) {
  using namespace boost::ut;

  ctx.cwd = ffilesystem::get_cwd();

  ctx.dir = ctx.cwd + "/ffs_test_" + std::string{test_name} + "_dir";

  ctx.in_dir = "./invalid-memory-trailing-non-null-terminated-string_view";
  ctx.nonnull_dir = std::string_view(ctx.in_dir.data(), 2);
  expect(ctx.nonnull_dir.back() != '\0' >> fatal) << "nonnull_dir should not be null-terminated\n";
}

} // namespace

int main() {
  using namespace boost::ut;

  if (!ffilesystem::is_writable(".")) {
    skip / "mkdir"_test = [] {};
  } else {
  "mkdir"_test = [] {
    mkdir_ctx ctx;
    setup(ctx, "mkdir");

    expect(!ffilesystem::mkdir(""));

    // Test mkdir with existing directory
    expect(ffilesystem::mkdir(ctx.dir) >> fatal);

    // Test mkdir with relative path
    expect(ffilesystem::set_cwd(ctx.dir) >> fatal);

    expect(ffilesystem::mkdir("test-filesystem-dir/hello") >> fatal);
    expect(ffilesystem::is_directory(ctx.dir + "/test-filesystem-dir/hello"));

    expect(ffilesystem::mkdir(ctx.nonnull_dir) >> fatal);
    expect(!ffilesystem::is_directory(ctx.in_dir));
  };
}
}
