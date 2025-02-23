// verify functions handle empty input OK

#include <string>

#include "ffilesystem.h"

#include <gtest/gtest.h>

TEST(TestEmpty, EmptyInput){

std::string_view e = "";

EXPECT_TRUE(fs_as_posix(e).empty());
EXPECT_TRUE(fs_file_name(e).empty());
EXPECT_TRUE(fs_stem(e).empty());
EXPECT_TRUE(fs_join(e, e).empty());
EXPECT_TRUE(fs_suffix(e).empty());
EXPECT_TRUE(fs_with_suffix(e, e).empty());
EXPECT_FALSE(fs_is_char_device(e));
EXPECT_FALSE(fs_is_reserved(e));
EXPECT_FALSE(fs_is_symlink(e));
EXPECT_FALSE(fs_create_symlink(e, e));
EXPECT_FALSE(fs_mkdir(e));
EXPECT_TRUE(fs_which(e).empty());
EXPECT_TRUE(fs_root(e).empty());
EXPECT_FALSE(fs_exists(e));
EXPECT_FALSE(fs_is_absolute(e));
EXPECT_FALSE(fs_is_dir(e));
EXPECT_FALSE(fs_is_exe(e));
EXPECT_FALSE(fs_is_file(e));
EXPECT_FALSE(fs_remove(e));
EXPECT_TRUE(fs_canonical(e, false, false).empty());
EXPECT_FALSE(fs_equivalent(e, e));
EXPECT_TRUE(fs_expanduser(e).empty());
EXPECT_FALSE(fs_copy_file(e,e, false));
EXPECT_FALSE(fs_touch(e));
EXPECT_EQ(fs_file_size(e), 0);
EXPECT_FALSE(fs_get_cwd().empty());
EXPECT_FALSE(fs_get_homedir().empty());

if(!fs_is_windows()){

  EXPECT_EQ(fs_space_available(e), 0);
  EXPECT_FALSE(fs_set_permissions(e, 0, 0, 0));
}

}
