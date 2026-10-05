#include <string>
#include <string_view>

#include <system_error>

#include "ffilesystem.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#if __has_include(<format>)
#include <format> // IWYU pragma: keep
#endif

#endif

#if __has_include(<sys/utsname.h>)
#define HAVE_UTSNAME
#include <sys/utsname.h>
#endif


bool fs_is_wsl()
{
  // false: not WSL, or the probe failed
  // Uses three-step heuristic to detect WSL
  // 1. Check for WSL-specific environment variables (WSL_DISTRO_NAME, WSL_INTEROP)
  // 2. Check for the presence of the /run/WSL directory
  // 3. Check the uname release string for "microsoft" or "wsl"
  // The first two checks are set by /init, and so don't rely on Microsoft's kernel
  // However, the user could have cleared the environment variables, so we have
  // fallbacks.
#if defined(__linux__)
  auto env_is_set = [](std::string_view name) {
    const auto value = fs_getenv(name);
    return value && !value->empty();
  };
  if (env_is_set("WSL_DISTRO_NAME") || env_is_set("WSL_INTEROP"))
    return true;

  if (fs_is_dir("/run/WSL"))
    return true;

#ifdef HAVE_UTSNAME
  struct utsname b;
  if (::uname(&b) != 0 || std::string_view(b.sysname) != "Linux")
    return false;

  std::string r(b.release);
  fs_ascii_lower(r);

#if defined(__cpp_lib_string_contains) // C++23
  return r.contains("microsoft") || r.contains("wsl");
#else
  return r.find("microsoft") != std::string::npos ||
         r.find("wsl") != std::string::npos;
#endif

#endif
#endif
  return false;
}


std::string fs_cpu_arch()
{

  std::error_code ec;

#ifdef HAVE_UTSNAME
  if (struct utsname b; ::uname(&b) == 0)
    return b.machine;
#elif defined(_WIN32)
// https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/ns-sysinfoapi-system_info
    SYSTEM_INFO si;
    GetNativeSystemInfo(&si);
    switch (si.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64: return "x86_64";
    case PROCESSOR_ARCHITECTURE_ARM64: return "arm64";
    case PROCESSOR_ARCHITECTURE_ARM: return "arm";
    case PROCESSOR_ARCHITECTURE_INTEL: return "x86";
    default: return "unknown";
    }
#else
  ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_error_callback("", ec);
  return {};
}


std::string fs_os_version()
{
// get operating system version

  std::error_code ec;

#ifdef HAVE_UTSNAME
  if (struct utsname buf; ::uname(&buf) == 0)
    return buf.version;
#elif defined(_WIN32)
 // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlgetversion
  using RtlGetVersionFn = LONG(WINAPI*)(PRTL_OSVERSIONINFOW);

  HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
  if (!ntdll) {
    fs_error_callback("ntdll.dll not found");
    return {};
  }
  auto fn = reinterpret_cast<RtlGetVersionFn>(GetProcAddress(ntdll, "RtlGetVersion"));
  if (!fn) {
    fs_error_callback("RtlGetVersion not found");
    return {};
  }

  RTL_OSVERSIONINFOW vi{};
  vi.dwOSVersionInfoSize = sizeof(vi);
  if (fn(&vi) != 0) {
    fs_error_callback("RtlGetVersion failed");
    return {};
  }

  const auto major = vi.dwMajorVersion;
  const auto minor = vi.dwMinorVersion;
  const auto build = vi.dwBuildNumber;
#if defined(__cpp_lib_format)  // C++20
    return std::format("{}.{}.{}", major, minor, build);
#else
    return std::to_string(major) + '.' + std::to_string(minor) + '.' + std::to_string(build);
#endif

#else
  ec = std::make_error_code(std::errc::function_not_supported);
#endif

  fs_error_callback("", ec);
  return {};
}
