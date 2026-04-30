#include <string>
#include <fstream>
#include <string_view>

#include "ffilesystem.h"

#include <boost/ut.hpp>

namespace {

struct filesize_ctx {
  std::string file;
  std::string in_file;
  std::string_view nonnull_file;

  ~filesize_ctx() {
    if (!file.empty()) {
      fs_remove(file);
    }
  }
};

auto setup(filesize_ctx& ctx) -> bool {
  using namespace boost::ut;

  if (!fs_is_writable(".")) {
    return false;
  }

  ctx.file = "ffs_filesize_5bytes.txt";
  std::ofstream ofs(ctx.file);
  ofs << "hello";

  ctx.in_file = ctx.file + "-read_past_the_end_of_buffer";
  ctx.nonnull_file = std::string_view(ctx.in_file.data(), ctx.file.size());
  expect(ctx.nonnull_file.back() != '\0' >> fatal);

  return true;
}

} // namespace

int main() {
  using namespace boost::ut;

  "file_size"_test = [] {
    filesize_ctx ctx;
    if (!setup(ctx)) {
      return;
    }

    expect(eq(fs_file_size(ctx.file), static_cast<std::size_t>(5)));

    expect(eq(fs_file_size("."), fs_unknown_size)) << "backend " << fs_backend();
    expect(eq(fs_file_size("not-exist-file"), fs_unknown_size)) << "backend " << fs_backend();

    expect(eq(fs_file_size(ctx.nonnull_file), static_cast<std::size_t>(5)))
        << "fs_file_size() non-null-terminated path";
  };
}
