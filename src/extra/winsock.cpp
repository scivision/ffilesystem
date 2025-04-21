#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <unistd.h>
#endif

#include <string>

#include "ffilesystem.h"


std::string fs_hostname()
{
  std::string name(fs_get_max_path(), '\0');
  int ret = -1;

#if defined(_WIN32)
  if(WSADATA wsaData; !WSAStartup(MAKEWORD(2, 0), &wsaData)){
    ret = gethostname(name.data(), static_cast<int>(name.size()));
    WSACleanup();
  }

#else
  ret = gethostname(name.data(), name.size());
#endif

  if (ret == 0)
    return fs_trim(name);

  fs_print_error(name, __func__);
  return {};
}
