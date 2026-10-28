#include "ffilesystem.h"
#include <string>

#include <boost/ut.hpp>

namespace {

struct set_cwd_ctx {
  std::string tmp;
  std::string in_dir;
  std::string_view nonnull_dir;
};

auto setup(set_cwd_ctx& ctx) {
  using namespace boost::ut;

  ctx.tmp = ffilesystem::drop_slash(ffilesystem::get_tempdir());
  expect(ffilesystem::is_directory(ctx.tmp) >> fatal);

  ctx.in_dir = "./invalid-memory-trailing-non-null-terminated-string_view";
  ctx.nonnull_dir = std::string_view(ctx.in_dir.data(), 2);
  expect(ctx.nonnull_dir.back() != '\0' >> fatal) << "nonnull_dir should not be null-terminated\n";
}

} // namespace

int main() {
  using namespace boost::ut;

  "set_cwd"_test = [] {
    set_cwd_ctx ctx;
    setup(ctx);

    expect(!ffilesystem::set_cwd(""));

    expect(ffilesystem::set_cwd(ctx.tmp) >> fatal);
    // needs to be ffilesystem::equivalent due to links, network drives, etc.
    expect(ffilesystem::equivalent(ffilesystem::get_cwd(), ctx.tmp))
        << "cwd " << ffilesystem::get_cwd() << " != " << ctx.tmp << " canonical " << ffilesystem::canonical(ctx.tmp);

    expect(ffilesystem::set_cwd(ctx.nonnull_dir) >> fatal) << "problem with non null-terminated path " << ctx.nonnull_dir;
    expect(ffilesystem::equivalent(ffilesystem::get_cwd(), ctx.tmp));
  };
}
