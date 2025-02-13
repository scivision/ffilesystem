#include <string>
#include <string_view>
#include <iostream>

#if __has_include(<ranges>)
#include <ranges>  // IWYU pragma: keep
#endif

#include <algorithm> // std::transform, std::ranges::contains, std::find, std::replace
#include <cctype> // std::isalnum, toupper
#include <array>

#include "ffilesystem.h"


std::string fs_as_posix(std::string_view path)
{
  std::string s(path);

  if(fs_is_windows())
    std::replace(s.begin(), s.end(), '\\', '/');

  return s;
}


bool
fs_is_reserved(std::string_view path)
{
  // defined as https://docs.python.org/3.13/library/os.path.html#os.path.isreserved
  if(!fs_is_windows())
    return false;

  const std::string n = fs_file_name(path);

  if(n.empty() || n == ".")
    return false;

  if(n.back() == '.' || n.back() == ' ')
    return true;

  // return true if filename contains any of :*?"<>| or ends with a space or period
  // https://docs.microsoft.com/en-us/windows/win32/fileio/naming-a-file

  if(n.find_first_of(R"(<>:"/\|?*)") != std::string::npos)
    return true;

  // don't detect ASCII control characters as reserved, since multi-byte characters may falsely trip that check

  std::string s = fs_stem(n);

  if(size_t L = s.length();
      L < 3 || L > 4)
    return false;

  // convert to upper case
  std::transform(s.begin(), s.end(), s.begin(), ::toupper);

// check if the stem is a reserved device name
  constexpr std::array<std::string_view, 30> r = {"CON", "PRN", "AUX", "NUL",
    "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "COM¹", "COM²", "COM³",
    "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9", "LPT¹", "LPT²", "LPT³",
    "CONIN$", "CONOUT$"};

#ifdef __cpp_lib_ranges_contains  // C++23
  if (std::ranges::contains(r, s))
#elif defined(__cpp_lib_ranges) // C++20
  if (std::ranges::find(r, s) != r.end())
#else // C++98
  if (std::find(r.begin(), r.end(), s) != r.end())
#endif
    return true;

return false;
}


static bool
fs_is_safe_char(const char c)
{
  // unordered_set<char>  8us
  // set<char, std::less<>>  6us
  // vector<char> 0.3us so much faster!
  // <string_view>.find_first_of is same speed as vector<char> but more readable
  static constexpr std::string_view safe = "_-.~@#$%^&()[]{}+=,!";

  return std::isalnum(c) || safe.find_first_of(c) != std::string::npos;
}


bool
fs_is_safe_name(std::string_view filename)
{
  // check that only shell-safe characters are in filename
  // hasn't been fully tested yet across operating systems and file systems--let us know if character(s) should be unsafe
  // does NOT check for reserved or device names
  // => filenames only, not paths
  // https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file
  // we do not consider whitespaces, quotes, or ticks safe, as they can be confusing in shell scripts and command line usage

  if(fs_is_reserved(filename))
    return false;

#ifdef __cpp_lib_ranges // C++20
  return std::ranges::all_of(filename, fs_is_safe_char);
#else // C++11
  return std::all_of(filename.begin(), filename.end(), fs_is_safe_char);
#endif
}


bool
fs_non_ascii(std::string_view name)
{
  // check if name contains non-ASCII characters

#ifdef __cpp_lib_ranges // C++20
  return !std::ranges::all_of(name, [](int c) { return std::isprint(c); });
#else // C++11
  return !std::all_of(name.begin(), name.end(), [](int c) { return std::isprint(c); });
#endif

}


bool
fs_is_prefix(std::string_view prefix, std::string_view path)
{
  // is prefix a prefix of path. Does not normalize, canonicalize, or walk up the directory tree.
  // ".." is ambiguous and should be avoided in input

  if(prefix.empty() || path.empty())
    return false;

  const std::string pr = fs_drop_slash(prefix);
  const std::string p = fs_drop_slash(path);

  if (pr == p)
    return true;

  if (pr.size() > p.size())
    return false;

#ifdef __cpp_lib_starts_ends_with  // C++20
  return p.starts_with(pr);
#else
  return p.substr(0, pr.size()) == pr;
#endif

}


bool
fs_is_subdir(std::string_view subdir, std::string_view dir)
{
  // is subdir a subdirectory of dir. Does not normalize, canonicalize, or walk up the directory tree.
  // ".." is ambiguous and should be avoided in input

  if(subdir.empty() || dir.empty())
    return false;

  const std::string s = fs_drop_slash(subdir);
  const std::string d = fs_drop_slash(dir);

  if(s == d)
    return false;

  if(s.size() < d.size())
    return false;

#ifdef __cpp_lib_starts_ends_with  // C++20
  return s.starts_with(d);
#else
  return s.substr(0, d.size()) == d;
#endif

}
