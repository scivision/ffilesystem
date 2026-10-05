#include "ffilesystem.h"

#include <boost/ut.hpp>
#include <limits>
#include <string>

#ifdef FFS_SYSTEM_DISABLED
namespace {
std::string error_message;

void capture_error(const char* message)
{
  error_message = message;
}
}
#endif

int main() {
using namespace boost::ut;

#ifdef FFS_SYSTEM_DISABLED
"disabled_memory"_test = [] {
  fs_set_error_callback(capture_error);
  expect(eq(fs_get_free_memory(), std::numeric_limits<unsigned long long>::max()));
  expect(error_message.find("system support disabled") != std::string::npos);
  error_message.clear();
  expect(eq(fs_total_sys_memory(), 0ULL));
  expect(error_message.find("system support disabled") != std::string::npos);
  fs_reset_error_callback();
};
#else
"memory"_test = [] {
  expect(gt(fs_get_free_memory(), 0));
};

"total_memory"_test = [] {
  expect(gt(fs_total_sys_memory(), 0));
};

"free_less_than_total"_test = [] {
  expect(le(fs_get_free_memory(), fs_total_sys_memory()));
};
#endif

"max_open_files"_test = [] {
  expect(gt(fs_get_max_open_files(), 0));
};
}
