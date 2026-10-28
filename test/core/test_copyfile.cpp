#include <string>
#include <fstream>
#include <cstdint>

#include "ffilesystem.h"

#include <boost/ut.hpp>

namespace {

struct copyfile_ctx {
  std::string s1, s2, s3, s4, t1;
  std::string ext1, ext5;
  std::uintmax_t iref;
  std::string cwd;
};

auto make_ctx() {
  using namespace boost::ut;
  copyfile_ctx ctx{};

  ctx.cwd = ffilesystem::get_cwd();
  expect(!ctx.cwd.empty() && ffilesystem::is_writable(ctx.cwd) >> fatal);

  std::string n = "TestCopyFile";

  ctx.s1 = ctx.cwd + ffilesystem::file_separator + n + "_some_text.txt";
  ctx.s2 = ctx.cwd + ffilesystem::file_separator + n + "_some_text.txt.copy";
  ctx.s3 = ctx.cwd + ffilesystem::file_separator + n + "_empty.txt";
  ctx.s4 = ctx.cwd + ffilesystem::file_separator + n + "_empty.txt.copy";
  if (ffilesystem::is_windows && fs_win32_long_paths_enabled()) {
    ctx.ext1 = R"(\\?\)" + ctx.s1;
    ctx.ext5 = R"(\\?\)" + ctx.s2 + ".long";
  }

  ctx.t1 = "及せゃ市人購ゅトてへ投際ト点吉で速流つ今日";

  // Write to the first file
  std::ofstream ofs(ctx.s1);
  if (!ofs) {
    return std::optional<copyfile_ctx>{};
  }
  ofs << ctx.t1;
  ofs.close(); // ensure flush

  ctx.iref = ffilesystem::file_size(ctx.s1);
  expect(ctx.iref > 0 >> fatal);

 ffilesystem::remove(ctx.s2);

  expect(ffilesystem::touch(ctx.s3) >> fatal);

  return std::optional<copyfile_ctx>{ctx};
}

} // namespace

int main() {
  using namespace boost::ut;

  "copyfile"_test = [] {
    const auto ctx = make_ctx();
    expect(ctx.has_value() >> fatal);

    std::string t2;

    // Copy the file
    expect(ffilesystem::copy_file(ctx->s1, ctx->s2, false));
    expect(ffilesystem::is_file(ctx->s2));
    expect(!ffilesystem::copy_file(ctx->s1, ctx->s2, false)); // Should fail since s2 already exists

    expect(eq(ffilesystem::file_size(ctx->s2), ctx->iref));

    // Read from the copied file
    std::ifstream ifs(ctx->s2);
    std::getline(ifs, t2);
    ifs.close();

    expect(eq(ctx->t1, t2));
    expect(!ffilesystem::copy_file(ctx->s1, ctx->s1, true));
    expect(eq(ffilesystem::file_size(ctx->s1), ctx->iref));

    expect(ffilesystem::copy_file(ctx->s3, ctx->s2, true));
    expect(eq(ffilesystem::file_size(ctx->s2), static_cast<std::uintmax_t>(0)));

    // Test copying from a file that does not exist
    const std::string missing = ctx->s1 + ".missing";
    expect(!ffilesystem::copy_file(missing, ctx->s2, true));
    expect(eq(ffilesystem::file_size(ctx->s2), static_cast<std::uintmax_t>(0)));

    expect(ffilesystem::copy_file(ctx->s3, ctx->s4, true));
    expect(ffilesystem::is_file(ctx->s4));

    expect(eq(ffilesystem::file_size(ctx->s4), static_cast<std::uintmax_t>(0)));

    if (!ctx->ext5.empty()) {
      expect(ffilesystem::copy_file(ctx->ext1, ctx->ext5, false) >> fatal);
      expect(ffilesystem::is_file(ctx->ext5) >> fatal);
      expect(eq(ffilesystem::file_size(ctx->ext5), ctx->iref));
    }

    // Cleanup
   ffilesystem::remove(ctx->s1);
   ffilesystem::remove(ctx->s2);
   ffilesystem::remove(ctx->s3);
   ffilesystem::remove(ctx->s4);
    if (!ctx->ext5.empty()) {
     ffilesystem::remove(ctx->ext5);
    }
  };
}
