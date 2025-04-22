#include <string>
#include <string_view>

#include <cstdlib>

// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/getenv-s-wgetenv-s

std::string fs_getenv_s(std::string_view name)
{
  std::string::size_type L;
  getenv_s(&L, nullptr, 0, name.data());
  if (L == 0)
    return {};
  if (auto buf = std::getenv(name.data()); buf)
    return buf;

  std::string buf(L, '\0');
  getenv_s(&L, &buf[0], L, name.data());
  buf.resize(L - 1); // remove the null terminator
}
