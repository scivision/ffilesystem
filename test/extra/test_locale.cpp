#include <string>

#include "ffilesystem.h"

#include <gtest/gtest.h>


TEST(TestLocale, LocaleName){
  std::string loc = fs_get_locale_name();
  // macOS and MSVC have empty locale. Don't fail
  if(!fs_is_macos() && !fs_is_msvc()){
    EXPECT_FALSE(loc.empty());
  }

}
