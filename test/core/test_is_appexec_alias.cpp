#include "ffilesystem.h"

#include <string>

#include <boost/ut.hpp>

namespace {

struct app_exec_ctx {
  std::string path;
};

bool setup(app_exec_ctx& ctx) {
  using namespace boost::ut;

  std::string appdir{fs_getenv("LOCALAPPDATA").value_or("")};

  if (!appdir.empty()) {
    appdir += "\\Microsoft\\WindowsApps";
    expect(fs_is_dir(appdir) >> fatal) << "app execution alias directory not found " << appdir;
  }

  for (const auto& exe : {"wt.exe", "winget.exe", "wsl.exe", "bash.exe"}) {
    ctx.path = fs_which(exe, appdir);
    // ARM Clang MSYS2 always gets empty from fs_which with path specified. Has to be in this directory to be appexec alias
    std::cout << "Checking for app execution alias: " << exe << " in " << appdir << " got " << ctx.path << "\n";
    if (!ctx.path.empty()) {
      break;
    }
  }

  return !ctx.path.empty();
}

} // namespace

int main() {
  using namespace boost::ut;

  app_exec_ctx ctx;

if (!fs_is_windows() || !setup(ctx)) {
  skip / "app_exec_alias"_test = [] {};
  return 77;
} else {
  "app_exec_alias"_test = [=] {
    expect(fs_is_appexec_alias(ctx.path))  << "failed on " << ctx.path;
  };
}
}
