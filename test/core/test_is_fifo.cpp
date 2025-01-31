#include "ffilesystem.h"
#include "ffilesystem_test.h"

#include <string_view>

#include <iostream>
#include <cstdlib>
#include <string>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> // getpid
#endif

#if __has_include(<format>)
#include <format>
#endif


int main()
{

std::string name;

#if defined(_WIN32)
// https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createnamedpipea

// must have this path prefix or INVALID_HANDLE_VALUE results
#ifdef __cpp_lib_format  // C++20
  name = std::format(R"(\\.\pipe\test_pipe_{})", GetCurrentProcessId());
#else
  name = R"(\\.\pipe\test_pipe_)" + std::to_string(GetCurrentProcessId());
#endif

HANDLE hPipe = CreateNamedPipeA(name.data(),
                             PIPE_ACCESS_DUPLEX,
                             PIPE_TYPE_BYTE,
                             1,
                             0, 0, 0, nullptr);

if (hPipe == INVALID_HANDLE_VALUE)
  err("test_is_fifo:CreateNamedPipeA(" + name + ")");

#else

#ifdef __cpp_lib_format
  name = std::format("test_pipe_{}", getpid());
#else
  name = "test_pipe_" + std::to_string(getpid());
#endif

  if (mkfifo(name.c_str(), 0666) == -1)
    err("mkfifo(" + name + ")");
#endif

if (!fs_is_fifo(name))
  err("is_fifo(" + name + ") should be true");

#if defined(_WIN32)
if (!CloseHandle(hPipe))
  err("CloseHandle(" + name + ")");
#endif

ok_msg("is_fifo");

return EXIT_SUCCESS;

}
