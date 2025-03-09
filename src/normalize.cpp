#include "ffilesystem.h"

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
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
#ifdef HAVE_CXX_FILESYSTEM
  r = std::filesystem::path(path).lexically_normal().generic_string();
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
  std::string s = fs_as_posix(in);

  while(s.length() > 1 && s.back() == '/')
    s.pop_back();

  if (fs_is_windows() && !in.empty() && s == fs_root_name(in))
    s.append("/");

  if(fs_trace > 1) std::cout << "TRACE:drop_slash(" << in << "): removed trailing slash: " << s << std::endl;

  s.erase(std::unique(s.begin(), s.end(), [](char a, char b){ return a == '/' && b == '/'; }), s.end());

  if(fs_trace > 1) std::cout << "TRACE:drop_slash(" << in << "): removed duplicated internal slashes: " << s << std::endl;

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
  std::string p = fs_as_posix(path);

  // split path, including last component
  std::string::size_type start = 0;
  std::string::size_type end;

  while (start < p.length()) {
    end = p.find('/', start);
    if(fs_trace) std::cout << "TRACE:split(" << path << "): " << start << " " << end << " " << p.substr(start, end-start) << "\n";

    // last component
    if (end == std::string::npos){
      parts.push_back(p.substr(start));
      break;
    }

    // do not add empty parts
    if (end != start)
      parts.push_back(p.substr(start, end - start));

    start = end + 1;
  }

  if (fs_trace) std::cout << "TRACE:split(" << path << "): number of parts: " << parts.size() << "\n";

  return parts;
}
