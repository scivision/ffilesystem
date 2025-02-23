#include "ffilesystem.h"

#include <string_view>
#include <string>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>  // mkfifo
#include <unistd.h> // getpid
#endif

#if __has_include(<format>)
#include <format>
#endif

#include <gtest/gtest.h>

class TestFifo : public testing::Test {
  protected:
    std::string name;
#if defined(_WIN32)
    HANDLE hPipe;
#endif
    void SetUp() override {
#if defined(_WIN32)
      // https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createnamedpipea

      // must have this path prefix or INVALID_HANDLE_VALUE results
#ifdef __cpp_lib_format  // C++20
      name = std::format(R"(\\.\pipe\test_pipe_{})", GetCurrentProcessId());
#else
      name = R"(\\.\pipe\test_pipe_)" + std::to_string(GetCurrentProcessId());
#endif
      hPipe = CreateNamedPipeA(name.data(),
                                PIPE_ACCESS_DUPLEX,
                                PIPE_TYPE_BYTE,
                                1,
                                0, 0, 0, nullptr);

      ASSERT_NE(hPipe, INVALID_HANDLE_VALUE);
#else

#ifdef __cpp_lib_format
      name = std::format("test_pipe_{}", getpid());
#else
      name = "test_pipe_" + std::to_string(getpid());
#endif

      ASSERT_NE(mkfifo(name.c_str(), 0666), -1);
#endif
    }

    void TearDown() override {
#if defined(_WIN32)
      CloseHandle(hPipe);
#endif
    }
};


TEST_F(TestFifo, IsFIFO)
{
  EXPECT_TRUE(fs_is_fifo(name));
}
