#include <string>
#include <string_view>

#include <iostream>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "ffilesystem.h"


static std::string fs_which_generic(std::string_view name, std::string_view path, const bool find_all)
{
  if (fs_is_exe(name))
    return fs_as_posix(name);

  // relative directory component, but path was not a file
  if(fs_file_name(name).length() != name.length())
    return {};

  std::string paths = path.empty() ? fs_getenv("PATH") : std::string(path);

  if(paths.empty()){
    fs_print_error(paths, "which: search path is empty");
    return {};
  }

  paths = fs_as_posix(paths);

  if(fs_trace) std::cout << "TRACE:which: search path: " << paths << "\n";

  std::string n(name);
  std::string r;
  std::string t;

  std::string::size_type start = 0;
  std::string::size_type end;

  while (true) {
    end = paths.find(fs_pathsep(), start);

    // avoid empty path
    if (end != std::string::npos && end == start){
      start = end + 1;
      continue;
    }

    std::string dir = (end == std::string::npos)
      ? paths.substr(start)
      : paths.substr(start, end - start);

    r = dir + "/" + n;

    if (fs_trace) std::cout << "TRACE:which(" << r << "): is_file " << fs_is_file(r) << " is_exe " << fs_is_exe(r) << "\n";

    if (fs_is_file(r) && fs_is_exe(r)){
      if(find_all)
        t += r + fs_pathsep();
      else
        return fs_as_posix(r);
    }

    if(end == std::string::npos)
      break;

    start = end + 1;
  }

  if(find_all && !t.empty()){
    t.pop_back();
    return t;
  }

  return {};
}


std::string fs_which(std::string_view name, std::string_view path, const bool find_all)
{
  // find an executable file "name" under each path in
  // environment variable PATH or "path" if specified

  if (!path.empty() && !fs_is_dir(path)){
    fs_print_error(path, "which: path is not a directory");
    return {};
  }

#if defined(_WIN32)
  // use SearchPathA, even though the generic method works.
  // This is because SearchPathA uses registry preferences that the generic method ignores.

  if(find_all)
    return fs_which_generic(name, path, true);

  std::string r(fs_get_max_path(), '\0');
  DWORD L;

  // https://learn.microsoft.com/en-us/windows/win32/api/processenv/nf-processenv-searchpatha
  if (path.empty())
    L = SearchPathA(nullptr, name.data(), ".exe", static_cast<DWORD>(r.length()), r.data(), nullptr);
  else
    L = SearchPathA(path.data(), name.data(), ".exe", static_cast<DWORD>(r.length()), r.data(), nullptr);

  if(L == 0 && GetLastError() == ERROR_FILE_NOT_FOUND)
    return {};

  if(L == 0 || L >= r.length()){
    fs_print_error(name, "which");
    return {};
  }
  r.resize(L);

  if(fs_trace) std::cout << "TRACE: which: SearchPathA: " << r << "  length " << L << "\n";
  if(!fs_is_exe(r))
    return {};

  return fs_as_posix(r);

#else
  return fs_which_generic(name, path, find_all);
#endif
}
