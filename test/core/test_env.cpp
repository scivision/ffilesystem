#include <string>
#include <string_view>

#include "ffilesystem.h"

#include <boost/ut.hpp>
#ifdef FFS_SYSTEM_DISABLED
#include "disabled_system.h"
#endif

int main() {
  using namespace boost::ut;

  "username"_test = [] {
#ifdef FFS_SYSTEM_DISABLED
    ffs_test::expect_disabled_string([] { return ffilesystem::get_username(); });
#else
    std::string user = ffilesystem::get_username();
    expect(!user.empty());
    std::cout << "Username " << user << "\n";
#endif
  };

  "environment"_test = [] {
    expect(!ffilesystem::get_cwd().empty());

#ifdef FFS_SYSTEM_DISABLED
    ffs_test::expect_disabled_string([] { return ffilesystem::get_profile_dir(); });
#else
    std::string pdir = ffilesystem::get_profile_dir();
    expect(!pdir.empty());
    std::cout << "Profile directory " << pdir << "\n";
#endif

    std::string p = ffilesystem::get_homedir();
    expect(!p.empty());
    std::cout << "Home directory " << p << "\n";
    expect(ffilesystem::is_directory(p));

    // NOTE: profiledir does not always (but may) equal homedir, for example when root user.

    expect(eq(ffilesystem::expanduser("~"), p));

    // --- tempdir
    auto t = ffilesystem::get_tempdir();
    expect(!t.empty());
    std::cout << "Temp directory " << t << "\n";
    expect(ffilesystem::is_directory(t));
  };

  "getenv"_test = [] {
    const std::string name = ffilesystem::is_windows ? "USERPROFILE" : "HOME";
    std::string in_name = name + "-walkoff-end-of-buffer";
    std::string_view nonnull_name(in_name.data(), name.size());
    expect(nonnull_name.back() != '\0') << "Environment variable name should not be null-terminated in test";

    auto h = ffilesystem::getenv(ffilesystem::is_windows ? "USERPROFILE" : "HOME");
    expect(h.has_value() >> fatal) << "Environment variable HOME or USERPROFILE should be set";

    h = ffilesystem::getenv(nonnull_name);
    expect(h.has_value()) << "Environment variable " << nonnull_name << " should be set in test";
  };

  "setenv"_test = [] {
    const std::string k = "FORTtest";
    const std::string v = "FORTvalue";
    ffilesystem::setenv(k, v);

    auto e = ffilesystem::getenv(k);
    expect(e.has_value() >> fatal) << "Environment variable " << k << " not set";
    expect(eq(e.value(), v));

    ffilesystem::setenv(k, ""); // unset
    e = ffilesystem::getenv(k);
    expect(!e.has_value()) << "Environment variable " << k << " should be unset";

    std::string in_k = k + "-walkoff-end-of-buffer";
    std::string_view nonnull_k(in_k.data(), k.size());
    expect(nonnull_k.back() != '\0' >> fatal) << "Environment variable name should not be null-terminated in test";

    expect(ffilesystem::setenv(nonnull_k, v) >> fatal) << "Failed to set environment variable with non-null-terminated name";
    e = ffilesystem::getenv(k);
    expect(e.has_value() >> fatal) << "Environment variable " << k << " not set with non-null-terminated name";
    expect(eq(e.value(), v)) << "Environment variable " << k << " has wrong value with non-null-terminated name";
  };

  "no_environment"_test = [] {
    expect(ffilesystem::setenv("PATH", "") >> fatal);
    expect(ffilesystem::setenv("HOME", "") >> fatal);
    expect(ffilesystem::setenv("XDG_CONFIG_HOME", "") >> fatal);
    expect(ffilesystem::setenv("TMPDIR", "") >> fatal);
    if (ffilesystem::is_windows) {
      expect(ffilesystem::setenv("USERPROFILE", "") >> fatal);
    }

#ifdef FFS_SYSTEM_DISABLED
    ffs_test::expect_disabled_string([] { return ffilesystem::get_profile_dir(); });
#else
    std::string pdir = ffilesystem::get_profile_dir();
    expect(!pdir.empty());
    std::cout << "Profile directory " << pdir << "\n";
#endif

    auto t = ffilesystem::get_tempdir();
    expect(!t.empty());
    std::cout << "Temp directory " << t << "\n";
    expect(ffilesystem::is_directory(t));
  };

  "no_home"_test = [] {
    expect(ffilesystem::setenv("HOME", "") >> fatal);
    expect(ffilesystem::setenv("XDG_CONFIG_HOME", "") >> fatal);
    if (ffilesystem::is_windows) {
      expect(ffilesystem::setenv("USERPROFILE", "") >> fatal);
    }

    auto h = ffilesystem::getenv(ffilesystem::is_windows ? "USERPROFILE" : "HOME");
    expect(!h.has_value()) << "Environment variable HOME or USERPROFILE should not be set in test";

#ifdef FFS_SYSTEM_DISABLED
    ffs_test::expect_disabled_string([] { return ffilesystem::get_homedir(); });
    ffs_test::expect_disabled_string([] { return ffilesystem::expanduser("~"); });
#else
    std::string p = ffilesystem::get_homedir();
    expect(!p.empty());
    std::cout << "Home directory " << p << "\n";
    expect(ffilesystem::is_directory(p));

    // NOTE: profiledir does not always (but may) equal homedir, for example when root user.

    expect(eq(ffilesystem::expanduser("~"), p));
#endif
  };
}
