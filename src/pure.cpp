#include <string>
#include <string_view>

#include <system_error>
#include <cstring>

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#elif defined(_WIN32)
#include <cstdlib> // _splitpath_s, _MAX_DRIVE
#endif

#include "ffilesystem.h"


char fs_pathsep(){
  return fs_is_windows() ? ';' : ':';
}


const char* fs_devnull(){
  return fs_is_windows() ? "nul" : "/dev/null";
}


bool
fs_slash_first(std::string_view path)
{
  return !path.empty() && (path.front() == '/' || (fs_is_windows() && path.front() == '\\'));
}


std::string::size_type fs_strncpy(const char* path, char* result, const std::string::size_type buffer_size)
{
// check size before copy
  std::string::size_type L = std::strlen(path);
  if(L >= buffer_size){
    fs_print_error(path, __func__, std::make_error_code(std::errc::result_out_of_range));
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


bool fs_is_absolute(std::string_view path)
{
  // is path absolute?

#if defined(HAVE_CXX_FILESYSTEM) && !defined(__MINGW32__)
  // MinGW GCC <filesystem> .is_absolute doesn't handle UNC paths at least through GCC 15.1
  return std::filesystem::path(path).is_absolute();
#else
  if(fs_is_windows()) {
    if(path.length() < 3)
      return false;

    // Extended-length or device path
    if(path.length() >= 4 && (path.substr(0, 4) == R"(\\?\)" || path.substr(0, 4) == R"(\\.\)"))
        return true;

    // UNC path
    if (path.substr(0, 2) == R"(\\)")
      return path.find(R"(\)", 2) != std::string::npos;

    // Windows drive letter
    return !(fs_root_name(path).empty()) && (path[2] == '/' || path[2] == '\\');
  } else {
    return !path.empty() && path.front() == '/';
  }
#endif
}


std::string fs_file_name(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).filename().generic_string();
#else

  const auto i = path.find_last_of(fs_is_windows() ? "/\\" : "/");

  return (i != std::string::npos)
    ? std::string(path.substr(i + 1))
    : std::string(path);
#endif
}


std::string fs_root(std::string_view path)
{
  // root_path = root_name / root_directory

#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).root_path().generic_string();
#else
  if (std::string r = fs_root_name(path);
       r.empty())
    return fs_slash_first(path) ? "/" : "";
  else
    return (fs_is_windows() && r == path) ? r : r + "/";
#endif
}


std::string fs_root_name(FFS_MAYBE_UNUSED std::string_view path)
{

#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).root_name().string();
#elif defined(_WIN32)
  char drive[_MAX_DRIVE];
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/splitpath-s-wsplitpath-s
  if(_splitpath_s(path.data(), drive, _MAX_DRIVE, nullptr, 0, nullptr, 0, nullptr, 0) == 0)
    return fs_trim(drive);
#endif
  return {};
}


std::string fs_stem(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).filename().stem().generic_string();
#else
  std::string r = fs_file_name(path);
  // handle special case a/..
  if (r == "..")
    return r;

  // find last dot
  std::string::size_type i = r.find_last_of('.');

  return (i != std::string::npos && i != 0)
    ? r.substr(0, i)
    : r;
#endif
}


std::string fs_suffix(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).filename().extension().generic_string();
#else
  const std::string p = fs_file_name(path);
  // find last dot
  std::string::size_type i = p.find_last_of('.');

  return (i != std::string::npos && i != 0)
    ? p.substr(i)
    : "";
#endif
}


std::string fs_join(std::string_view path, std::string_view other)
{
  // does not normalize to preserve possible symlinks
  if(path.empty() && other.empty())
    return {};
  if(path.empty())
    return fs_drop_slash(other);
  if(other.empty())
    return fs_drop_slash(path);

  std::string p = fs_drop_slash(path);
  const std::string o = fs_drop_slash(other);

#ifdef HAVE_CXX_FILESYSTEM
  return (std::filesystem::path(p) / o).generic_string();
#else
  if (o.front() == '/' || (fs_is_windows() && fs_is_absolute(o)))
    return o;

  p.push_back('/');

  return p.append(o);
#endif
}


std::string fs_with_suffix(std::string_view path, std::string_view new_suffix)
{
  const std::string stem = fs_stem(path);
  // handle directory case: stem is empty
  if(stem.empty())
    return fs_join(path, new_suffix);

#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).replace_extension(new_suffix).generic_string();
#else
  const std::string p = fs_parent(path);

  const std::string r = (p == ".") ? stem : p + "/" + stem;

  return r + std::string(new_suffix);

#endif
}


std::string fs_lexically_normal(std::string_view path){
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).lexically_normal().generic_string();
#else
  fs_print_error(path, __func__, std::make_error_code(std::errc::function_not_supported));
  return {};
#endif
}


std::string fs_make_preferred(std::string_view path){
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).make_preferred().generic_string();
#else
  fs_print_error(path, __func__, std::make_error_code(std::errc::function_not_supported));
  return {};
#endif
}
