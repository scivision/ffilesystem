#include "ffilesystem.h"

#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Filesystem = std::filesystem;
#endif

#if defined(HAVE_CPP_RANGES_SPLIT_STRING_VIEW)
#include <ranges>
#endif


#include <algorithm> // std::unique
#include <string>
#include <string_view>

#include <vector>
#include <iostream>


namespace {

#if !defined(HAVE_CXX_FILESYSTEM)
// split posix-converted "p" (backed by path) into non-empty components, dropping "." and resolving ".."
// views reference "p", so "p" must outlive the returned vector
std::vector<std::string_view>
fs_normal_split(std::string_view path, const std::string& p)
{
  std::vector<std::string_view> parts;

  std::string_view::size_type start{0};
  while (start < p.length()) {
    auto end = p.find('/', start);
    if (end == std::string_view::npos)
      end = p.length();

    const std::string_view part(p.data() + start, end - start);
    start = end + 1;

    if (part.empty() || part == ".")
      continue;

    if (part == "..") {
      if (!parts.empty() && parts.back() != "..")
        parts.pop_back();
      else if (path.front() != '/')
        parts.push_back(part);
      continue;
    }

    parts.push_back(part);
  }

  return parts;
}
#endif

} // namespace


std::string
fs_normal(std::string_view path)
{
  // normalize path
   std::string r;
#if defined(HAVE_CXX_FILESYSTEM)
  r = Filesystem::path(path).lexically_normal().generic_string();
#else
  // leave the empty short-circuit to avoid needless computation
  // and avoid indexing into an empty string
  if (path.empty())
    return ".";

  const std::string p{fs_as_posix(path)};
  const std::vector<std::string_view> parts = fs_normal_split(path, p);

  // rebuild path
  r.reserve(p.length() + 1);  // avoid reallocation while appending below

   if (fs_slash_first(path))
     r.push_back('/');

  for (const auto& part : parts){
    r.append(part);
    r.push_back('/');
  }

#endif

  // no trailing slash
  if (r.length() > 1 &&
      (r.back() == '/' || r.back() == fs_filesep()) &&
      (!fs_is_windows() || r != fs_root(r))
    )
    r.pop_back();

  if (r.empty())
    r.push_back('.');

  return r;
}


void
fs_drop_trailing_slash(std::string& p)
{
  // drop trailing "/" and "\" from the path
  // but not if it is the Windows root name

  const std::string rn = fs_root_name(p);

  while(p.length() > 1 && (p.back() == '/' || p.back() == fs_filesep()))
    p.pop_back();

  if (fs_is_windows() && !p.empty() && p == rn)
    p.push_back('/');
}


std::string
fs_drop_slash(std::string_view in)
{
  // drop all trailing "/" and duplicated internal "/"

  if(in.empty())
    return {};

  // UNC, extended, and device paths are outside the supported normalization model.
#if defined(_WIN32)
  if (in.length() >= 2 && in[0] == '\\' && in[1] == '\\')
    return std::string(in);
#endif

  std::string s(in);

  fs_as_posix(s);

  fs_drop_trailing_slash(s);

  if(fs_trace > 1) std::cout << "TRACE:drop_slash(" << in << "): removed trailing slash: " << s << "\n";

  s.erase(std::unique(s.begin(), s.end(), [](char a, char b){ return a == '/' && b == '/'; }), s.end());

  if(fs_trace > 1) std::cout << "TRACE:drop_slash(" << in << "): removed duplicated internal slashes: " << s << "\n";

  return s;
}


void
fs_trim(std::string& s)
{
  // remove all content after the first null character
  if(auto i = s.find('\0'); i != std::string::npos)
    s.resize(i);
}


std::vector<std::string>
fs_split(std::string_view path)
{
  if(path.empty())
    return {};

  std::string const p{fs_as_posix(path)};

  // break paths into non-empty components
  std::vector<std::string> parts;

#if defined(HAVE_CPP_RANGES_SPLIT_STRING_VIEW)
  for (const auto& sub : p | std::views::split('/')) {
    std::string_view part(sub.begin(), sub.end());
    if (!part.empty())
      parts.emplace_back(part);
  }
  if (fs_trace) std::cout << "TRACE:fs_split(" << path << "): used C++20 range adapter\n";
#else
  // split path, including last component
  std::string_view::size_type start{0};
  std::string_view::size_type end;

  while (start < p.length()) {

    end = p.find('/', start);

    if(fs_trace) std::cout << "TRACE:split(" << p << "): " << start << " " << end << " " << p.substr(start, end-start) << "\n";

    // last component
    if (end == std::string_view::npos){
      parts.push_back(p.substr(start));
      break;
    }

    // do not add empty parts
    if (end != start)
      parts.push_back(p.substr(start, end - start));

    start = end + 1;
  }
#endif

  if (fs_trace) std::cout << "TRACE:split(" << path << "): number of parts: " << parts.size() << "\n";

  return parts;
}

std::vector<std::string>
fs_split_pathsep(std::string_view path)
{
  // break paths into elements separated by path separator,
  // excluding empty elements.
  std::vector<std::string> parts;

  // split path, including last component
  std::string_view::size_type start{0};
  std::string_view::size_type end;

  while (start < path.length()) {
    end = path.find(fs_pathsep(), start);

    if (end == std::string_view::npos){
      parts.push_back(std::string(path.substr(start)));
      break;
    }

    if (end != start)
      parts.push_back(std::string(path.substr(start, end - start)));

    start = end + 1;
  }

  return parts;
}
