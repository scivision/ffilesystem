#include <iostream>
#include <cstdlib>
#include <string>
#include <string_view>
#include <vector>
#include <functional>
#include <map>
#include <ctime>
#include <system_error>

#include <chrono> // IWYU pragma: keep
// needed to std::format() std::filesystem::file_time_type

#if __has_include(<format>)
#include <format>
#endif

#if defined(HAVE_BOOST_FILESYSTEM)
#include <boost/filesystem.hpp>
#elif defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
#endif

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <crtdbg.h>
#endif

#include "ffilesystem.h"


static void no_arg(std::string_view fun){

  // map string to function call using std::map std::function

  std::map<std::string_view, std::function<bool()>> mbool =
  {
    {"optimized", fs_is_optimized},
    {"is_admin", fs_is_admin},
    {"is_bsd", fs_is_bsd},
    {"is_linux", fs_is_linux},
    {"is_macos", fs_is_macos},
    {"is_rosetta", fs_is_rosetta},
    {"is_unix", fs_is_unix},
    {"is_windows", fs_is_windows},
    {"is_mingw", fs_is_mingw},
    {"is_cygwin", fs_is_cygwin},
    {"has_statx", fs_has_statx}
  };


  if (fun == "backend")
    std::cout << fs_backend() << "\n";
  else if (fun == "is_wsl")
    std::cout << fs_is_wsl() << "\n";
  else if (fun == "pid")
    std::cout << fs_getpid() << "\n";
  else if (fun == "pathsep")
    std::cout << fs_pathsep() << "\n";
  else if (fun == "lang")
    std::cout << fs_lang() << "\n";
  else if (fun == "locale")
    std::cout << fs_get_locale_name() << "\n";
  else if (fun == "username")
    std::cout << fs_get_username() << "\n";
  else if (fun == "hostname")
    std::cout << fs_hostname() << "\n";
  else if (fun == "shell")
    std::cout << fs_get_shell() << "\n";
  else if (fun == "stdin_tty")
    std::cout << fs_stdin_tty() << "\n";
  else if (fun == "arch")
    std::cout << fs_cpu_arch() << "\n";
  else if (fun == "profile")
    std::cout << fs_get_profile_dir() << "\n";
  else if (fun == "config")
    std::cout << fs_user_config_dir() << "\n";
  else if (fun == "libpath")
    std::cout << fs_lib_path() << "\n";
  else if (fun == "exepath")
    std::cout << fs_exe_path() << "\n";
  else if (fun == "compiler")
    std::cout << fs_compiler() << "\n";
  else if (fun == "homedir")
    std::cout << fs_get_homedir() << "\n";
  else if (fun == "terminal")
    std::cout << fs_get_terminal() << "\n";
  else if (fun == "tempdir")
    std::cout << fs_get_tempdir() << "\n";
  else if (fun == "max_path")
    std::cout << fs_get_max_path() << "\n";
#if __cplusplus >= 202002L
  else if (mbool.contains(fun))
#else
  else if (mbool.find(fun) != mbool.end())
#endif
    std::cout << mbool[fun]() << "\n";
  else
    std::cerr << fun << " not a known function\n";

}

static void one_arg(std::string_view fun, std::string_view a1){


  Fserr::error_code ec;

  if (fun == "lexically_normal")
    std::cout << fs_lexically_normal(a1) << "\n";
  else if (fun == "make_preferred")
    std::cout << fs_make_preferred(a1) << "\n";
  else if (fun == "parent")
    std::cout << fs_parent(a1) << "\n";
  else if (fun == "suffix")
    std::cout << fs_suffix(a1) << "\n";
  else if (fun == "canonical")
    std::cout << fs_canonical(a1, true, false) << "\n";
  else if (fun == "weakly_canonical")
    std::cout << fs_canonical(a1, false, false) << "\n";
  else if (fun == "resolve")
    std::cout << fs_resolve(a1, true, false) << "\n";
  else if (fun == "weakly_resolve")
    std::cout << fs_resolve(a1, false, false) << "\n";
  else if (fun == "hard")
    std::cout << fs_hard_link_count(a1) << "\n";
  else if (fun == "parts"){
    for (const auto &p : fs_split(a1))
      std::cout << p << "\n";
  }
  else if (fun == "normal")
    std::cout << fs_normal(a1) << "\n";
  else if (fun == "mkdtemp")
    std::cout << fs_mkdtemp(a1) << "\n";
  else if (fun == "type")
    std::cout << fs_filesystem_type(a1) << "\n";
  else if (fun == "is_reserved")
    std::cout << fs_is_reserved(a1) << "\n";
  else if (fun == "is_safe")
    std::cout << fs_is_safe_name(a1) << "\n";
  else if (fun == "long")
    std::cout << fs_longname(a1) << "\n";
  else if (fun == "short")
    std::cout << fs_shortname(a1) << "\n";
  else if (fun == "touch")
    std::cout << "touch " << a1 << " " << fs_touch(a1) << "\n";
  else if (fun == "getenv")
    std::cout << fs_getenv(a1) << "\n";
  else if (fun == "realpath")
    std::cout << fs_realpath(a1) << "\n";
  else if (fun == "posix")
    std::cout << fs_as_posix(a1) << "\n";
  else if (fun == "max_component")
    std::cout << fs_max_component(a1) << "\n";
  else if (fun == "mkdir")
    std::cout << fs_mkdir(a1) << "\n";
  else if (fun == "which")
    std::cout << fs_which(a1) << "\n";
  else if (fun == "owner"){
    std::cout << fs_get_owner_name(a1) << "\n";
    std::cout << fs_get_owner_group(a1) << "\n";
  }
  else if (fun == "expanduser")
    std::cout << fs_expanduser(a1) << "\n";
  else if (fun == "final_path")
    std::cout << fs_win32_final_path(a1) << "\n";
  else if (fun == "root")
    std::cout << fs_root(a1) << "\n";
  else if (fun == "root_name")
    std::cout << fs_root_name(a1) << "\n";
  else if (fun == "filename")
    std::cout << fs_file_name(a1) << "\n";
  else if (fun == "file_size")
    std::cout << fs_file_size(a1) << "\n";
  else if (fun == "is_absolute")
    std::cout << fs_is_absolute(a1) << "\n";
  else if (fun == "is_exe")
    std::cout << fs_is_exe(a1) << "\n";
  else if (fun == "is_case_sensitive")
    std::cout << fs_is_case_sensitive(a1) << "\n";
  else if (fun == "is_dir")
    std::cout << fs_is_dir(a1) << "\n";
  else if (fun == "is_char")
    std::cout << fs_is_char_device(a1) << "\n";
  else if (fun == "is_file")
    std::cout << fs_is_file(a1) << "\n";
  else if (fun == "is_symlink")
    std::cout << fs_is_symlink(a1) << "\n";
  else if (fun == "is_readable")
    std::cout << fs_is_readable(a1) << "\n";
  else if (fun == "is_writable")
    std::cout << fs_is_writable(a1) << "\n";
  else if (fun == "perm")
    std::cout << fs_get_permissions(a1) << "\n";
  else if (fun == "read_symlink")
    std::cout << fs_read_symlink(a1) << "\n";
  else if (fun == "stem")
    std::cout << fs_stem(a1) << "\n";
  else if (fun == "exists")
    std::cout << fs_exists(a1) << "\n";
  else if (fun == "space")
    std::cout << fs_space_available(a1) << "\n";
  else if (fun == "absolute")
    std::cout << fs_absolute(a1, true)<< "\n";
  else if (fun == "get_cwd")
    std::cout << fs_get_cwd() << "\n";
  else if (fun == "is_empty")
    std::cout << fs_is_empty(a1) << "\n";
  else if (fun == "modtime"){
#if defined(HAVE_CXX_FILESYSTEM) && defined(__cpp_lib_format)
    const auto t = fs_get_modtime_fs(a1);
    if(t)
      std::cout << std::format("{}\n", t.value());
#else
    const auto t = fs_get_modtime(a1.data());
    #if defined(_MSC_VER)
      std::string buf(26, '\0');
      ctime_s(buf.data(), buf.size(), &t);
      std::cout << buf << "\n";
    #else
      std::cout << std::ctime(&t) << "\n"; // NOSONAR
    #endif
#endif
  } else if (fun == "fs_modtime")
    std::cout << fs_get_modtime(a1.data()) << "\n";
  else if (fun == "chdir" || fun == "set_cwd") {
    auto cwd = fs_get_cwd();
    if(!cwd.empty()){
      std::cout << "cwd: " << cwd << "\n";
      if(fs_set_cwd(a1)){
        cwd = fs_get_cwd();
        if(!cwd.empty())
          std::cout << "new cwd: " << cwd << "\n";
        else
          std::cerr << "ERROR get_cwd() after chdir\n";
      }
    } else {
      std::cerr << "ERROR get_cwd() before chdir\n";
    }
  } else if (fun == "ls") {
#if defined(HAVE_CXX_FILESYSTEM) || defined(HAVE_BOOST_FILESYSTEM)
    for (auto const& dir_entry : Fs::directory_iterator{fs_expanduser(a1)}){
      Fs::path p = dir_entry.path();
      std::cout << p;
      if (const auto &s = Fs::file_size(p, ec); s && !ec)
        std::cout << " " << s;

      std::cout << " " << fs_get_permissions(p.generic_string()) << "\n";
    }
#else
      std::cerr << "ERROR: ls requires C++17 filesystem\n";
#endif
  } else {
    std::cerr << fun << " requires more arguments or is unknown function\n";
  }
}


static void two_arg(std::string_view fun, std::string_view a1, std::string_view a2)
{

  if (fun == "same")
    std::cout << fs_equivalent(a1, a2) << "\n";
  else if (fun == "join")
    std::cout << fs_join(a1, a2) << "\n";
  else if (fun == "with_suffix")
    std::cout << fs_with_suffix(a1, a2) << "\n";
  else if (fun == "is_subdir")
    std::cout << fs_is_subdir(a1, a2) << "\n";
  else if (fun == "relative")
    std::cout << fs_relative_to(a1, a2) << "\n";
  else if (fun == "proximate")
    std::cout << fs_proximate_to(a1, a2) << "\n";
  else if (fun == "setenv"){
    fs_setenv(a1, a2);
    std::cout << "set env var " << a1 << " to " << a2 << " => " << fs_getenv(a1) << "\n";
  } else if (fun == "copy")
    fs_copy_file(a1, a2, false);
  else if (fun == "create_symlink")
    fs_create_symlink(a1, a2);
  else if (fun == "absolute"){
    std::cout << fs_absolute(a1, a2, true)<< "\n";
  }
  else
    std::cerr << fun << " requires more arguments or is unknown function\n";

}

static void four_arg(std::string_view fun, std::string_view a1, std::string_view a2, std::string_view a3, std::string_view a4){
  if (fun == "set_perm"){
    int r = std::stoi(a2.data());
    int w = std::stoi(a3.data());
    int x = std::stoi(a4.data());

    auto p = fs_get_permissions(a1);
    if(p.empty())
      std::cerr << "ERROR get_permissions(" << a1 << ") before chmod\n";
    else
      std::cout << "before chmod " << a1 << " " << p << "\n";

    fs_set_permissions(a1, r, w, x);

    if(p.empty())
      std::cerr << "ERROR get_permissions(" << a1 << ") after chmod\n";
    else
      std::cout << "after chmod " << a1 << " " << p << "\n";

  } else {
    std::cerr << fun << " requires more arguments or is unknown function\n";
  }

}


int main(){
#ifdef _MSC_VER
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

const std::string cwd = fs_get_cwd();

std::cout << "Ffilesystem. Backend: " << fs_backend() << "\n";
std::cout << "maximum path length: " << fs_get_max_path() << "\n";
std::cout << "maximum path component length: " << fs_max_component(cwd) << "\n";
std::cout << "current working directory (CWD): " << cwd << "\n";
std::cout << "Compiler: " << fs_compiler() << "\n";
std::cout << "Optimized: " << fs_is_optimized() << " Trace: " << FS_TRACE << "\n";
std::cout << "CPU arch: " << fs_cpu_arch() << "\n";
std::cout << "C++ " << fs_lang() << "\n";
std::cout << "Locale: " << fs_get_locale_name() << "\n";
std::cout << "Username: " << fs_get_username() << "\n";
std::cout << "Hostname: " << fs_hostname() << "\n";
std::cout << "Shell: " << fs_get_shell() << "\n";
std::cout << "Terminal: " << fs_get_terminal() << "\n";
std::cout << "Homedir: " << fs_get_homedir() << "\n";
std::cout << "CWD filesystem Type: " << fs_filesystem_type(cwd) << "\n";

if(fs_is_admin())
  std::cerr << "WARNING: running as admin / sudo" << std::endl;

std::cout << std::endl;  // flush for CI etc.

while (true){

  std::string inp;

  std::cout << "Ffs> ";

  std::getline(std::cin, inp);

  // "\x04" is Ctrl-D on Windows.
  // EOF for non-Windows
  if (std::cin.eof() || inp ==
#ifdef __cpp_named_character_escapes
  "\x{04}"
#else
  "\x04"
#endif
  || inp == "q" || inp == "quit" || inp == "exit")
    break;

  // split variable inp on space-delimiters
  constexpr char delimiter = ' ';
  std::string::size_type pos = 0;
  std::vector<std::string> args;
  // NOTE: loop getline() instead?
  while ((pos = inp.find(delimiter)) != std::string::npos) {
      args.push_back(inp.substr(0, pos));
      inp.erase(0, pos + 1);  // + 1 as delimiter is 1 char
  }
  // last argument
  args.push_back(inp);

  if(args.empty())
    continue;

  const std::vector<std::string>::size_type argc = args.size();

  switch (argc){
  case 1:
    no_arg(args.at(0));
    break;
  case 2:
    one_arg(args.at(0), args.at(1));
    break;
  case 3:
    two_arg(args.at(0), args.at(1), args.at(2));
    break;
  case 5:
    four_arg(args.at(0), args.at(1), args.at(2), args.at(3), args.at(4));
    break;
  default:
    std::cerr << "too many arguments " << argc << "\n";
  }


}

return EXIT_SUCCESS;

}
