#include <gtest/gtest.h>

#include "ffilesystem.h"

#include <cctype> // for std::isalnum

TEST(Random, AlphanumericString)
{
  const std::string s = fs_generate_random_alphanumeric_string(16);
  EXPECT_EQ(s.size(), 16);
  for (char c : s) {
    EXPECT_TRUE(std::isalnum(static_cast<unsigned char>(c)));
  }
}
