#include "ffilesystem.h"

#include <string>

#include <boost/ut.hpp>

namespace {

struct remove_ctx {
  std::string file;
  std::string in2;
  std::string_view nonnull2;

  ~remove_ctx() {
    fs_remove(file);
  }
};

auto setup(remove_ctx& ctx) -> bool {
  using namespace boost::ut;
  if (!fs_is_writable(".")) {
    return false;
  }

  ctx.file = "ffs_remove_test.txt";
  expect(fs_touch(ctx.file) >> fatal);

  ctx.in2 = "./" + ctx.file;
  ctx.nonnull2 = std::string_view(ctx.in2.data(), 2);
  expect(ctx.nonnull2.back() != '\0' >> fatal) << "nonnull2 should not be null-terminated\n";
  return true;
}

} // namespace

int main() {
  using namespace boost::ut;

  "remove"_test = [] {
    remove_ctx ctx;
    if (!setup(ctx)) {
      return;
    }

    expect(!fs_remove(ctx.nonnull2)) << "Failed with input not null-terminated\n";
    expect(fs_remove(ctx.file));
  };
}
