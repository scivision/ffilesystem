#include "ffilesystem.h"

#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Filesystem = std::filesystem;
#endif


#include <algorithm> // std::unique
#include <string>
#include <string_view>

#include <vector>
#include <iostream>


std::vector<std::string>
fs_normal_vector(std::string_view path)
{
  if(path.empty())
    return {};

  const std::vector<std::string> parts = fs_split(path);

  std::vector<std::string> n;

  // drop repeated slashes, "." and ".."
  for (const auto& p : parts) {
    if (p.empty() || p == ".")
      continue;

    if (p == "..") {
      if (!n.empty() && n.back() != "..")
        n.pop_back();
      else if (path.front() != '/')
        n.push_back(p);
      continue;
    }

    n.push_back(p);
  }

  return n;
}


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

  const std::vector<std::string> parts = fs_normal_vector(path);

  // rebuild path
   if (fs_slash_first(path))
     r.push_back('/');

  for (const auto& p : parts)
    r.append(p).push_back('/');

#endif

  // no trailing slash
  if (r.length() > 1 && r.back() == '/' && (!fs_is_windows() || r != fs_root(r)))
    r.pop_back();

  if (r.empty())
    r.push_back('.');

  return r;
}


std::string
fs_drop_slash(std::string_view in)
{
  // drop all trailing "/" and duplicated internal "/"

  if(in.empty())
    return {};

  bool winPrefix = false;
  std::string::size_type i = std::string::npos;

  if(fs_is_windows()){
    // Extended-length or device path
    if(in.length() >= 4 && (in.substr(0, 4) == R"(\\?\)" || in.substr(0, 4) == R"(\\.\)")){
      i = 4;
    } else if (in.substr(0, 2) == R"(\\)"){
      i = in.find(R"(\)", 2);
    }
    winPrefix = i != std::string::npos;
  }

  std::string s(in);

  if (!winPrefix)
    s = fs_as_posix(s);

  while(s.length() > 1 && (s.back() == '/' || (fs_is_windows() && s.back() == '\\')))
    s.pop_back();

  if (fs_is_windows() && s == fs_root_name(in))
    s.push_back('/');

  if(fs_trace > 1) std::cout << "TRACE:drop_slash(" << in << "): removed trailing slash: " << s << "\n";

  s.erase(std::unique(s.begin(), s.end(), [](char a, char b){ return a == '/' && b == '/'; }), s.end());

  if(winPrefix){
    std::string t = s.substr(i);
    if(t == fs_root_name(in.substr(i)))
      t.push_back('/');

    t.erase(std::unique(t.begin(), t.end(), [](char a, char b){ return a == '\\' && b == '\\'; }), t.end());
    s = s.substr(0, i) + t;
  }

  if(fs_trace > 1) std::cout << "TRACE:drop_slash(" << in << "): removed duplicated internal slashes: " << s << "\n";

  return s;
}


std::string
fs_trim(std::string_view s)
{
  // trim trailing nulls
  std::string r(s);
  if(auto i = r.find('\0'); i != std::string::npos)
    r.resize(i);

  return r;
}


std::vector<std::string>
fs_split(std::string_view path)
{
  if(path.empty())
    return {};

  // break paths into non-empty components
  std::vector<std::string> parts;

  // split path, including last component
  std::string::size_type start = 0;
  std::string::size_type end;

  while (start < path.length()) {
    end = path.find_first_of(R"(/\)", start);
    if(fs_trace) std::cout << "TRACE:split(" << path << "): " << start << " " << end << " " << path.substr(start, end-start) << "\n";

    // last component
    if (end == std::string::npos){
      parts.push_back(std::string(path.substr(start)));
      break;
    }

    // do not add empty parts
    if (end != start)
      parts.push_back(std::string(path.substr(start, end - start)));

    start = end + 1;
  }

  if (fs_trace) std::cout << "TRACE:split(" << path << "): number of parts: " << parts.size() << "\n";

  return parts;
}
