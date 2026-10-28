#include "ffilesystem.h"

#include <boost/ut.hpp>

#include <string>
#include <algorithm> // for std::find

namespace {

struct which_ctx {
  std::string name;
  std::string dir;
  std::string rel;
  std::string testExe;
};

auto setup_which(which_ctx& ctx, std::string_view arg0) {
  using namespace boost::ut;

  ctx.testExe = ffilesystem::is_windows ? "cmake.exe" : "ls";

  const std::string self{arg0};
  expect(ffilesystem::is_exe(self) >> fatal);

  ctx.name = ffilesystem::file_name(self);
  ctx.dir = ffilesystem::parent(self);
  ctx.rel = std::string("./") + ctx.name;
  expect(ffilesystem::is_file(ctx.rel) >> fatal) << ctx.rel << " does not exist";
}

auto setup_which_no_path(which_ctx& ctx) {
  using namespace boost::ut;

  ctx.testExe = ffilesystem::is_windows ? "cmake.exe" : "ls";
  expect(ffilesystem::setenv("PATH", "") >> fatal);
}

} // namespace

int main(int argc, char** argv) {
  using namespace boost::ut;

  "which"_test = [argv] {
    which_ctx ctx;
    setup_which(ctx, argv[0]);

    expect(ffilesystem::is_absolute(ffilesystem::which(ctx.testExe)));
    expect(eq(ffilesystem::file_name(ffilesystem::which(ctx.testExe)), ctx.testExe));
    expect(ffilesystem::which(ctx.testExe, "nowhere").empty()) << ctx.testExe << " should not be found in 'nowhere'";

    expect(ffilesystem::which("/not/a/path").empty());
    expect(ffilesystem::which("").empty());

    // Build a relative path from the running program
    expect(!ffilesystem::which(ctx.rel).empty()) << ctx.rel << " is not found";

    expect(ffilesystem::which("not-exist/" + ctx.name).empty());
  };

  "which_local_dir"_test = [argv] {
    which_ctx ctx;
    setup_which(ctx, argv[0]);

    expect(ffilesystem::is_file(ctx.name) >> fatal) << ctx.name << " does not exist";
    // for Windows only: local dir is preferred
    if (ffilesystem::is_windows) {
      expect(!ffilesystem::which(ctx.name).empty());
    } else {
      expect(ffilesystem::which(ctx.name).empty());
    }

    expect(!ffilesystem::which(ctx.name, ctx.dir).empty()) << ctx.name << " should be found in " << ctx.dir;
  };

  "which_no_path"_test = [] {
    which_ctx ctx;
    setup_which_no_path(ctx);

    expect(ffilesystem::which(ctx.testExe).empty());
    expect(ffilesystem::which(ctx.testExe, "nowhere").empty());
  };
}
