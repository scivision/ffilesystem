#include "ffilesystem.h"

#include <iostream>
#include <cstdint>
#include <string>
#include <string_view>

#include <boost/ut.hpp>

namespace {

struct device_ctx {
  std::string in2, cwd;
  std::string_view nonnull2;
};

auto setup(device_ctx& ctx) {
  using namespace boost::ut;
  ctx.cwd = ffilesystem::get_cwd();
  ctx.in2 = ctx.cwd +"/invalid-memory-trailing-non-null-terminated-string_view";
  ctx.nonnull2 = std::string_view(ctx.in2.data(), ctx.cwd.size());
  expect(ctx.nonnull2.back() != '\0' >> fatal) << "nonnull2 should not be null-terminated\n";
}

} // namespace

int main() {
  using namespace boost::ut;

  "hard_link"_test = [] {
    device_ctx ctx;
    setup(ctx);

    expect(ffilesystem::hard_link_count(".") >= 1U);

    expect(eq(ffilesystem::hard_link_count("not-exist-file"), fs_unknown_size)) << "backend " << ffilesystem::backend;

    std::cout << "the return code for errors e.g. not existing file is " << fs_unknown_size << "\n";

    expect(neq(ffilesystem::hard_link_count(ctx.nonnull2), fs_unknown_size) >> fatal)
        << "ffilesystem::hard_link_count(" << ctx.nonnull2 << ") should not return error code " << fs_unknown_size;
    expect(ffilesystem::hard_link_count(ctx.nonnull2) >= 1U);
  };

  "blk_size"_test = [] {
    device_ctx ctx;
    setup(ctx);

    auto b1 = ffilesystem::get_blksize(ffilesystem::get_tempdir());
    expect(b1 > 0U);

    b1 = ffilesystem::get_blksize(ctx.cwd);
    expect(b1 > 0U);
    expect(eq(ffilesystem::get_blksize(ctx.nonnull2), b1))
        << "ffilesystem::get_blksize(" << ctx.nonnull2 << ") should be the same as ffilesystem::get_blksize(" << ffilesystem::get_tempdir() << ")";
  };

  "device"_test = [] {
    device_ctx ctx;
    setup(ctx);

    std::string_view in = "./";
    expect(ffilesystem::st_dev(in) > static_cast<dev_t>(0));

    expect(eq(ffilesystem::st_dev(ctx.nonnull2), ffilesystem::st_dev(in)))
        << "ffilesystem::st_dev(" << ctx.nonnull2 << ") should be the same as ffilesystem::st_dev(" << in << ")";

    expect(eq(ffilesystem::st_dev("not-exist-file"), static_cast<dev_t>(0))) << "backend " << ffilesystem::backend;
  };

  "inode"_test = [] {
    device_ctx ctx;
    setup(ctx);

    std::string_view in = "./";
    const auto inode_dot = ffilesystem::inode(in);
    expect(inode_dot > static_cast<ino_t>(0) >> fatal);

    expect(eq(ffilesystem::inode(ctx.nonnull2), inode_dot))
        << "ffilesystem::inode(" << ctx.nonnull2 << ") should be the same as ffilesystem::inode(" << in << ")";

    expect(eq(ffilesystem::inode("not-exist-file"), static_cast<ino_t>(0))) << "backend " << ffilesystem::backend;
  };
}
