#ifndef FFS_TEST_DISABLED_SYSTEM_H
#define FFS_TEST_DISABLED_SYSTEM_H

#include "ffilesystem.h"
#include <boost/ut.hpp>
#include <string>

namespace ffs_test {

inline std::string diagnostic;

inline void capture_diagnostic(const char* message)
{
  diagnostic += message;
}

template<typename Function>
void expect_disabled_string(Function function)
{
  diagnostic.clear();
  fs_set_error_callback(capture_diagnostic);
  const auto result = function();
  fs_reset_error_callback();
  boost::ut::expect(result.empty());
  boost::ut::expect(diagnostic.find(
    std::make_error_code(std::errc::function_not_supported).message()) != std::string::npos);
}

}

#endif
