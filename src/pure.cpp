#include <string>
#include <string_view>

#include <system_error>
#include <cstring>

#if defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
namespace Filesystem = std::filesystem;
#elif defined(_WIN32)
#include "win32_path.h"
#endif

#include "ffilesystem.h"


namespace ffilesystem {

bool is_absolute(std::string_view path)
{
  // is path absolute?

#if defined(HAVE_CXX_FILESYSTEM)
  return Filesystem::path(path).is_absolute();
#else
  return is_windows
    ? has_root_name(path) && has_root_directory(path)
    : has_root_directory(path);
#endif
}

}

const char* fs_devnull(){
  return ffilesystem::is_windows ? "nul" : "/dev/null";
}


std::string::size_type
fs_strncpy(const char* path, char* result, const std::string::size_type buffer_size)
{
// check size before copy
  const auto L = std::strlen(path);
  if(L >= buffer_size){
    fs_error_callback(path, std::make_error_code(std::errc::result_out_of_range));
    return 0;
  }

  if(L)
    #ifdef _MSC_VER
      strncpy_s(result, buffer_size, path, _TRUNCATE);
    #else
      std::strncpy(result, path, buffer_size);
    #endif

  return L;
}


namespace ffilesystem {

bool has_filename(std::string_view path)
{
  // does path have a filename component?
#if defined(HAVE_CXX_FILESYSTEM)
  return Filesystem::path(path).has_filename();
#else
  return !file_name(path).empty();
#endif
}


std::string file_name(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  return Filesystem::path(path).filename().string();
#else
  const auto i = path.find_last_of(file_separators);
  return std::string(path.substr(i == std::string_view::npos ? 0 : i + 1));
#endif
}


std::string root(std::string_view path)
{
  // root_path = root_name / root_directory

#ifdef HAVE_CXX_FILESYSTEM
  return Filesystem::path(path).root_path().string();
#else
  return root_name(path) + root_directory(path);
#endif
}


std::string root_directory(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  return Filesystem::path(path).root_directory().string();
#else
  if (is_windows) {
    const auto i = path.size() >= 2 && path[1] == ':' ? 2u : 0u;

    return i < path.size() && is_file_separator(path[i])
      ? std::string(1, path[i])
      : "";
  }

  return is_file_separator(path.front()) ? std::string(1, path.front()) : "";
#endif
}


bool has_root_directory(std::string_view path){
#ifdef HAVE_CXX_FILESYSTEM
  return Filesystem::path(path).has_root_directory();
#else
  return !root_directory(path).empty();
#endif
}


std::string root_name([[maybe_unused]] std::string_view path)
{

#ifdef HAVE_CXX_FILESYSTEM
  return Filesystem::path(path).root_name().string();
#else
  if (is_windows && path.size() >= 2 && path[1] == ':')
    return std::string(path.substr(0, 2));

  return {};
#endif
}

bool has_root_name(std::string_view path){
#ifdef HAVE_CXX_FILESYSTEM
  return Filesystem::path(path).has_root_name();
#else
  return !root_name(path).empty();
#endif
}


std::string stem(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  return Filesystem::path(path).filename().stem().string();
#else
  std::string r = file_name(path);
  // handle special case a/..
  if (r == "..")
    return r;

  // find last dot
  if (auto i = r.rfind('.'); i != std::string::npos && i != 0)
    r.resize(i);

  return r;
#endif
}


std::string suffix(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  return Filesystem::path(path).filename().extension().string();
#else
  const std::string p = file_name(path);
  // find last dot
  const auto i = p.rfind('.');

  return (i != std::string::npos && i != 0)
    ? p.substr(i)
    : "";
#endif
}


std::string join(std::string_view path, std::string_view other)
{
  // does not normalize to preserve possible symlinks

#ifdef HAVE_CXX_FILESYSTEM
  // use generic string to keep '/' file separator to avoid confusion
  return (Filesystem::path(path) / other).generic_string();
#else
  if(ffilesystem::is_absolute(other) || (has_root_name(other) && root_name(other) != root_name(path)))
    return std::string(other);
  else if (has_root_directory(other))
    return root_name(path).append(other);

  std::string p{path};

  if(!path.empty() && !is_file_separator(p.back()))
    p.push_back('/');

  return p.append(other);
#endif
}


std::string with_suffix(std::string_view path, std::string_view new_suffix)
{
  std::string const s{stem(path)};
  // handle directory case: stem is empty
  if(s.empty())
    return join(path, new_suffix);

#ifdef HAVE_CXX_FILESYSTEM
  return Filesystem::path(path).replace_extension(new_suffix).string();
#else
  std::string const p = parent(path);

  std::string r;
  if (p == ".")
    r = s;
  else {
    r = p;
    if(p.back() != '/')
      r.push_back('/');

    r += s;
  }

  return r.append(new_suffix);
#endif
}

}


std::string
fs_lexically_normal(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  return Filesystem::path(path).lexically_normal().generic_string();
#else
  fs_error_callback(path, std::make_error_code(std::errc::function_not_supported));
  return {};
#endif
}


std::string
fs_make_preferred(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  return Filesystem::path(path).make_preferred().string();
#else
  fs_error_callback(path, std::make_error_code(std::errc::function_not_supported));
  return {};
#endif
}
