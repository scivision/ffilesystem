#include "ffilesystem.h"

#include <string>

#include <boost/ut.hpp>

namespace {

struct app_exec_ctx {
  std::string path;
};

auto setup(app_exec_ctx& ctx) -> bool {
  using namespace boost::ut;

  if (!fs_is_windows()) {
    return false;
  }

  const std::string appdir = fs_getenv("LOCALAPPDATA").value_or("") + "/Microsoft/WindowsApps";
  expect(fs_is_dir(appdir) >> fatal);

  for (const auto& exe : {"wt.exe", "winget.exe", "wsl.exe", "bash.exe"}) {
    ctx.path = fs_which(exe, appdir);
    if (!ctx.path.empty()) {
      break;
    }
  }

  return !ctx.path.empty();
}

} // namespace

int main() {
  using namespace boost::ut;

  "app_exec_alias"_test = [] {
    app_exec_ctx ctx;
    if (!setup(ctx)) {
      return;
    }

    expect(fs_is_appexec_alias(ctx.path));
  };
}
