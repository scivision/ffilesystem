#include "ffilesystem.h"
#include <iostream>
#include <string>
#include <string_view>

#include <boost/ut.hpp>

namespace {

struct permissions_ctx {
  std::string read;
  std::string noread;
  std::string nowrite;
  std::string in_file;
  std::string_view nonnull_file;

  ~permissions_ctx() {
    fs_remove(read);
    fs_remove(noread);
    fs_remove(nowrite);
  }
};

auto setup(permissions_ctx& ctx, std::string_view test_name) -> bool {
  using namespace boost::ut;

  const std::string n = std::string{test_name};

  ctx.read = n + "readable.txt";
  ctx.noread = n + "nonreadable.txt";
  ctx.nowrite = n + "nonwritable.txt";

  if (!fs_is_writable(".")) {
    return false;
  }

  expect(fs_touch(ctx.read) >> fatal);
  expect(fs_is_file(ctx.read) >> fatal);

  expect(fs_touch(ctx.noread) >> fatal);
  expect(fs_is_file(ctx.noread) >> fatal);
  if (!fs_is_file(ctx.nowrite)) {
    expect(fs_touch(ctx.nowrite) >> fatal);
  }

  expect(fs_exists(ctx.nowrite) >> fatal);
  expect(fs_is_file(ctx.nowrite) >> fatal);

  ctx.in_file = ctx.read + "-read_past_the_end_of_buffer";
  ctx.nonnull_file = std::string_view(ctx.in_file.data(), ctx.read.size());
  expect(ctx.nonnull_file.back() != '\0' >> fatal);

  return true;
}

} // namespace

int main() {
  using namespace boost::ut;

  "permissions_empty"_test = [] {
    permissions_ctx ctx;
    if (!setup(ctx, "permissions_empty")) {
      return;
    }

    expect(fs_get_permissions("").empty());
    expect(fs_get_permissions("nonexistent.txt").empty());
    expect(!fs_get_permissions(ctx.read).empty());
  };

  "permissions_is_readable"_test = [] {
    permissions_ctx ctx;
    if (!setup(ctx, "permissions_is_readable")) {
      return;
    }

    expect(fs_is_readable(ctx.read)) << ctx.read << " should be readable";
  };

  "permissions_not_readable"_test = [] {
    permissions_ctx ctx;
    if (!setup(ctx, "permissions_not_readable")) {
      return;
    }

    // for Ffilesystem, even non-readable files "exist" and are "is_file"
    expect(fs_set_permissions(ctx.noread, -1, 0, 0) >> fatal);
    const std::string p = fs_get_permissions(ctx.noread);

    std::cout << "Permissions: " << ctx.noread << " " << p << "\n";

    if (!(fs_is_windows() || fs_is_cygwin())) {
      expect(eq(p[0], '-'));
    }
  };

  "permissions_read"_test = [] {
    permissions_ctx ctx;
    if (!setup(ctx, "permissions_read")) {
      return;
    }

    expect(fs_set_permissions(ctx.read, 1, 0, 0));
    expect(fs_is_readable(ctx.read));
    expect(fs_set_permissions(ctx.nonnull_file, 1, 0, 0));
  };

  "permissions_writable"_test = [] {
    permissions_ctx ctx;
    if (!setup(ctx, "permissions_writable")) {
      return;
    }

    // writable
    expect(fs_set_permissions(ctx.nowrite, 0, -1, 0) >> fatal);

    const std::string p = fs_get_permissions(ctx.nowrite);

    // MSVC with <filesystem>, but we'll skip all windows
    if (!fs_is_windows()) {
      expect(eq(p[1], '-'));

      if (!fs_is_admin()) {
        expect(!fs_is_writable(ctx.nowrite));
      }
    }
  };
}
