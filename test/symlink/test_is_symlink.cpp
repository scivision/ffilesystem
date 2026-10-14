#include "ffilesystem.h"
#include <iostream>

#include <boost/ut.hpp>

namespace {

struct symlink_ctx {
  std::string link_file;
  std::string link_file_rel;
  std::string link_dir;

  std::string tgt_file;
  std::string tgt_file_rel;
  std::string tgt_dir;

  std::string in_file;
  std::string_view nonnull_file;

  void cleanup() const {
    for (const auto& link : {link_file, link_dir, link_file_rel}) {
      if (fs_is_symlink(link)) {
        fs_remove(link);
      }
    }
    fs_remove(tgt_file);
    fs_remove(tgt_file_rel);
    fs_remove(tgt_dir);
  }

  // ~symlink_ctx() { cleanup(); }
};

auto setup(symlink_ctx& ctx) {
  using namespace boost::ut;

  const std::string cwd = fs_get_cwd();

  // absolute target dir
  ctx.tgt_dir = cwd + "/symlink_test_dir";
  expect(fs_mkdir(ctx.tgt_dir) >> fatal) << "Failed to create test directory: " << ctx.tgt_dir;

  // absolute target file
  ctx.tgt_file = ctx.tgt_dir + "/test_is_symlink_target.txt";
  expect(fs_touch(ctx.tgt_file) >> fatal) << "Failed to create target file: " << ctx.tgt_file;

  // The symlink target is relative to the link's parent directory.
  ctx.tgt_file_rel = "symlink_test_dir/test_is_symlink_rel.txt";
  expect(fs_touch(ctx.tgt_file_rel) >> fatal) << "Failed to create relative target file: " << ctx.tgt_file_rel;

  ctx.link_dir = ctx.tgt_dir + "/link.dir";
  ctx.link_file = ctx.link_dir + "/cmake_test_symlink.txt.link";
  ctx.link_file_rel = "test_is_symlink_rel.txt.link";

  // cleanup prior test
  for (const auto& link : {ctx.link_file, ctx.link_dir, ctx.link_file_rel}) {
    if (fs_is_symlink(link)) {
      std::cout << "Removing existing test symlink: " << link << "\n";
      expect(fs_remove(link) >> fatal) << "Failed to remove existing test symlink: " << link;
    }
  }

  // create absolute dir symlink
  expect(fs_create_symlink(ctx.tgt_dir, ctx.link_dir) >> fatal)
      << "Failed to create symlink: " << ctx.link_dir << " -> " << ctx.tgt_dir;
  expect(fs_is_dir(ctx.link_dir) >> fatal) << ctx.link_dir << " is not a directory";
  expect(fs_equivalent(ctx.link_dir, ctx.tgt_dir)) << "Symlink directory " << ctx.link_dir << " is not equivalent to target" << ctx.tgt_dir;

  // create absolute file symlink
  expect(fs_create_symlink(ctx.tgt_file, ctx.link_file) >> fatal)
      << "Failed to create symlink: " << ctx.link_file << " -> " << ctx.tgt_file;
  expect(fs_is_file(ctx.link_file) >> fatal) << ctx.link_file << " is not a file after creating symlink";
  expect(fs_equivalent(ctx.link_file, ctx.tgt_file)) << "Symlink file " << ctx.link_file << " is not equivalent to target" << ctx.tgt_file;

  // create relative file symlink
  expect(fs_create_symlink(ctx.tgt_file_rel, ctx.link_file_rel) >> fatal)
      << "Failed to create symlink: " << ctx.link_file_rel << " -> " << ctx.tgt_file_rel;
  expect(fs_is_file(ctx.link_file_rel) >> fatal) << ctx.link_file_rel << " is not a file after creating symlink";
  // fs_equivalent triggers failure of relative target on Windows before our src/symlink.cpp fix.
  expect(fs_equivalent(ctx.link_file_rel, ctx.tgt_file_rel)) << "Symlink file " << ctx.link_file_rel << " is not equivalent to target" << ctx.tgt_file_rel;

  ctx.in_file = ctx.link_file + "-read_past_the_end_of_buffer";
  ctx.nonnull_file = std::string_view(ctx.in_file.data(), ctx.link_file.size());
  expect(ctx.nonnull_file.back() != '\0' >> fatal);
}

} // namespace

int main() {
  using namespace boost::ut;

  "is_symlink_file"_test = [] {
    symlink_ctx ctx;
    setup(ctx);

    expect(!fs_is_symlink("not-exist-file"));
    expect(!fs_is_symlink(""));
    expect(fs_is_symlink(ctx.link_file));
    expect(fs_is_symlink(ctx.link_file_rel));

    expect(fs_is_symlink(ctx.nonnull_file)) << "is_symlink() should not read past the end of string_view buffer";
  };

  "is_symlink_dir"_test = [] {
    symlink_ctx ctx;
    setup(ctx);

    expect(fs_is_symlink(ctx.link_dir));
  };
}
