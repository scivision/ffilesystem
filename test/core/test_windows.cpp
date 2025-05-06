#include "ffilesystem.h"

#include <gtest/gtest.h>


TEST(Windows, ToWide)
{
  std::string_view s = "hello";
  std::wstring w = fs_win32_to_wide(s);

  EXPECT_EQ(w.size(), s.size());
  EXPECT_EQ(w, L"hello");
}


TEST(Windows, ToNarrow)
{
  std::wstring_view w = L"hello";
  std::string n = fs_win32_to_narrow(w);

  EXPECT_EQ(n.size(), w.size());
  EXPECT_EQ(n, "hello");
}
