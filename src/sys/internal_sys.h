

#if !defined(_WIN32)
#include <pwd.h>
#endif

#if !defined(_WIN32)
struct passwd* fs_getpwuid();
#endif
