#include <string>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <windows.h>
#include <tlhelp32.h> // for CreateToolhelp32Snapshot, Process32First
#else
#include <pwd.h>     // for passwd
#endif

#include "ffilesystem.h"


std::string
fs_get_shell()
{
#if defined(_WIN32)
  const HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (h == INVALID_HANDLE_VALUE) {
    fs_error_callback("");
    return {};
  }

  PROCESSENTRY32W pe{};
  pe.dwSize = sizeof(pe);
  DWORD parent_pid{0};

  if (Process32FirstW(h, &pe)) {
    const DWORD pid = GetCurrentProcessId();
    do {
      if (pe.th32ProcessID == pid) {
        parent_pid = pe.th32ParentProcessID;
        break;
      }
    } while (Process32NextW(h, &pe));
  }

  std::string name;
  pe = {};
  pe.dwSize = sizeof(pe);
  if (parent_pid != 0 && Process32FirstW(h, &pe)) {
    do {
      if (pe.th32ProcessID == parent_pid) {
        name = fs_win32_to_narrow(pe.szExeFile);
        break;
      }
    } while (Process32NextW(h, &pe));
  }

  CloseHandle(h);

  if (!name.empty()) {
    if (fs_trace) std::cout << "TRACE: get_shell: " << name << " PPID: " << parent_pid << "\n";
    return name;
  }
#else
  if (auto pw = fs_getpwuid())
    return pw->pw_shell;
#endif

  fs_error_callback("");
  return {};
}
