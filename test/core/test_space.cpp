#include "ffilesystem.h"
#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::AllOf;
using ::testing::Gt;
using ::testing::Lt;

// for Windows need an invalid drive as a non-existing relative path just gives the space anyway.

class TestSpace : public testing::Test {
  protected:
    std::string dir, in_dir;
    std::string_view nonnull_dir, nonnull_file;

    void SetUp() override {
      dir = fs_drop_slash(::testing::TempDir());
      std::cout << "Testing space on backend " << fs_backend() << " with temp dir " << dir << "\n";

      in_dir = dir + "-invalid-memory-trailing-non-null-terminated-string_view";
      nonnull_dir = std::string_view(in_dir.data(), dir.size());
      ASSERT_NE(nonnull_dir.back(), '\0') << "nonnull_dir should not be null-terminated\n";
    }
};

TEST_F(TestSpace, SpaceAvailable)
{
EXPECT_THAT(fs_space_available(dir), AllOf(Gt(0), Lt(fs_unknown_size)));

EXPECT_EQ(fs_space_available("cc:/not-exist-available"), fs_unknown_size) << "backend " << fs_backend();

EXPECT_NE(fs_space_available(nonnull_dir), fs_unknown_size) << "problem with non null-terminated path " << nonnull_dir;
}

TEST_F(TestSpace, SpaceCapacity)
{
EXPECT_THAT(fs_space_capacity(dir), AllOf(Gt(0), Lt(fs_unknown_size)));

EXPECT_EQ(fs_space_capacity("cc:/not-exist-capacity"), fs_unknown_size) << "backend " << fs_backend();

EXPECT_NE(fs_space_capacity(nonnull_dir), fs_unknown_size) << "problem with non null-terminated path " << nonnull_dir;
}
