#include <string>
#include <string_view>

#include "ffilesystem.h"

#include <boost/ut.hpp>

namespace {

struct equivalent_ctx {
  std::string self;
  std::string self_name;
  std::string in_file;
  std::string_view nonnull_file;
};

auto make_ctx(std::string_view arg0) {
  using namespace boost::ut;

    std::optional<equivalent_ctx> ctx{equivalent_ctx{}};

  ctx->self = arg0;
  ctx->self_name = ffilesystem::file_name(ctx->self);
  expect(ffilesystem::is_file(ctx->self) >> fatal) << "Test executable not found: " << ctx->self;

  ctx->in_file = ctx->self + "-read_past_the_end_of_buffer";
  ctx->nonnull_file = std::string_view(ctx->in_file.data(), ctx->self.size());
  expect(ctx->nonnull_file.back() != '\0') << "Test executable name should not end with null character";

  return ctx;
}

auto setup_inaccessible_directory(std::string& base, std::string& secret) {
  base = "fffilesystem::equiv_inaccessible_dir";
  secret = base + "/secret";

  if (ffilesystem::exists(secret) && !ffilesystem::set_permissions(secret, 1, 1, 1)) return false;

  if (ffilesystem::exists(base) && (!ffilesystem::remove(secret) || !ffilesystem::remove(base))) return false;

  if (!ffilesystem::mkdir(secret)) return false;
  if (!ffilesystem::set_permissions(base, -1, -1, -1)) return false;

  if(ffilesystem::exists(secret)) return false;
  if(ffilesystem::is_directory(secret)) return false;
  if(ffilesystem::is_file(secret)) return false;

  return true;
}

} // namespace

int main(int argc, char** argv) {
  using namespace boost::ut;

if (!ffilesystem::equivalent(".", ffilesystem::parent(argv[0]))) {
  skip / "equivalent_filename"_test = [] {};
} else {
  "equivalent_filename"_test = [argv] {
    const auto ctx = make_ctx(argv[0]);
    expect(ctx.has_value() >> fatal);

    expect(ffilesystem::is_file(ctx->self_name) >> fatal) << "Test executable name not found in CWD: " << ctx->self_name;

    expect(ffilesystem::equivalent(ctx->self_name, "./" + ctx->self_name));
    expect(ffilesystem::equivalent(ctx->self_name, ctx->self));
    expect(ffilesystem::equivalent(ctx->self_name, ctx->nonnull_file));
    expect(ffilesystem::equivalent(ctx->self, ctx->self_name));
    expect(ffilesystem::equivalent(ctx->self, ctx->self));
  };

  "equivalent_relative"_test = [] {
    std::string s = "fffilesystem::equiv_not-exist";
    expect(!ffilesystem::equivalent(s, s));

    std::string cwd = ffilesystem::get_cwd();
    expect(!cwd.empty() >> fatal);

    expect(ffilesystem::equivalent("..", ffilesystem::parent(cwd)));
    expect(ffilesystem::equivalent(".", "./"));
    expect(ffilesystem::equivalent(".", cwd));
    expect(!ffilesystem::equivalent("..", cwd));

    // NOTE: This can be false on networked drive or Windows Dev Drive
    // ffilesystem::equivalent(".", ffilesystem::realpath("."));
  };
}

if(ffilesystem::is_windows || fs_is_cygwin() || ffilesystem::is_admin() || !ffilesystem::is_writable(".")) {
    skip / "equivalent_inaccessible_directory"_test = [] {};
} else {
  std::string base;
  std::string secret;

  if (setup_inaccessible_directory(base, secret)) {
    "equivalent_inaccessible_directory"_test = [base, secret] {

      expect(!ffilesystem::equivalent(secret, secret));

      expect(ffilesystem::set_permissions(base, 1, 1, 1));
      expect(ffilesystem::remove(secret));
      expect(ffilesystem::remove(base));
    };
  } else {
    skip / "equivalent_inaccessible_directory"_test = [] {};
  }
}
}
