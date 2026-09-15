#include <sys/types.h>

#include <string_view>

#ifdef _MSC_VER
#ifndef mode_t
using mode_t = unsigned int;
#endif
#endif

mode_t fs_st_mode(std::string_view);
