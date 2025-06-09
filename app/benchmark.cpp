#include <chrono>
#include <string>
#include <algorithm>
#include <iostream>
#include <functional>
#include <cstdlib>
#include <vector>
#include <array>

#include <variant>
#include <unordered_map>

#include "ffilesystem.h"


void print_cpp(std::chrono::duration<double> t, int n, std::string_view path, std::string_view func, std::string_view w, bool b)
{
  std::chrono::nanoseconds ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t);
  double us = static_cast<double>(ns.count()) / 1000.0;
  std::cout << fs_backend() << ": " << n << " x " << func << "(" << path << ") = ";
  if(w.empty())
    std::cout << b;
  else
    std::cout << w;
  std::cout << ": " << us << " us\n";
}


std::chrono::duration<double> bench_cpp(int n, std::string_view path, std::string_view fname, bool verbose)
{

auto t = std::chrono::duration<double>::max();

constexpr bool strict = false;
constexpr bool expand_tilde = false;

using fs_function = std::function<std::variant<std::string, bool>(std::string_view)>;

std::unordered_map<std::string_view, fs_function> fs_function_map = {
  {"canonical", [=](std::string_view p) { return fs_canonical(p, strict, expand_tilde); }},
  {"resolve", [=](std::string_view p) { return fs_resolve(p, strict, expand_tilde); }},
  {"drop_slash", [](std::string_view p) { return fs_drop_slash(p); }},
  {"parent", [](std::string_view p) { return fs_parent(p); }},
  {"file_name", [](std::string_view p) { return fs_file_name(p); }},
  {"suffix", [](std::string_view p) { return fs_suffix(p); }},
  {"normal", [](std::string_view p) { return fs_normal(p); }},
  {"reserved", [](std::string_view p) { return fs_is_reserved(p); }},
  {"exists", [](std::string_view p) { return fs_exists(p); }},
  {"is_dir", [](std::string_view p) { return fs_is_dir(p); }},
  {"is_char", [](std::string_view p) { return fs_is_char_device(p); }},
  {"is_file", [](std::string_view p) { return fs_is_file(p); }},
  {"is_symlink", [](std::string_view p) { return fs_is_symlink(p); }},
  {"read_symlink", [](std::string_view p) { return fs_read_symlink(p); }},
  {"which", [](std::string_view p) { return fs_which(p); }},
  {"homedir", [](std::string_view) { return fs_get_homedir(); }},
  {"expanduser", [](std::string_view p) { return fs_expanduser(p); }},
  {"cwd", [](std::string_view) { return fs_get_cwd(); }},
  {"is_reserved", [](std::string_view p) { return fs_is_reserved(p); }}
};

// warmup
std::string h;
bool b = false;

  auto it = fs_function_map.find(fname);
  if (it != fs_function_map.end()) {
    auto result = it->second(path);
    if (std::holds_alternative<std::string>(result)){
      h = std::get<std::string>(result);
      if(h.empty()){
        std::cerr << "ERROR: " << fname << " " << path << " failed on warmup\n";
        return t;
      }
    } else
      b = std::get<bool>(result);
  } else {
    std::cerr << "Error: unknown function " << fname << "\n";
    return t;
  }

for (int i = 0; i < n; ++i)
{
  auto t0 = std::chrono::steady_clock::now();

  auto it = fs_function_map.find(fname);
  if (it != fs_function_map.end()) {
    auto result = it->second(path);
    if (std::holds_alternative<std::string>(result))
      h = std::get<std::string>(result);
    else
      b = std::get<bool>(result);
  } else {
    std::cerr << "Error: unknown function " << fname << "\n";
    return t;
  }

  auto t1 = std::chrono::steady_clock::now();
  t = std::min(t, std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0));
}

if(verbose)
  print_cpp(t, n, path, fname, h, b);

return t;
}


int main(int argc, char** argv){

if (!fs_is_optimized())
  std::cerr << "WARNING: ffilesystem might not have been compiled with optimizations\n";

int n = 1000;
if(argc > 1)
    n = std::stoi(argv[1]);

std::string_view path;

std::cout << fs_compiler() << "\n";

std::vector<std::string_view> funcs;
if(argc > 3)
  funcs = {argv[3]};
else
  funcs = {"canonical", "resolve", "which", "expanduser", "normal", "cwd", "homedir", "parent", "file_name", "is_reserved", "drop_slash"};

for (std::string_view func : funcs)
  {
  // in sorted ascending order for binary search
  constexpr std::array<std::string_view, 7> tf = {
    "canonical", "drop_slash", "expanduser", "file_name", "normal", "parent", "resolve"
  };

  if (argc > 2)
    path = argv[2];
  else {
    if (std::binary_search(tf.begin(), tf.end(), func))
      path = "~/..";
    else if (func == "which")
      path = (fs_is_windows()) ? "cmake.exe" : "sh";
    else
      path = "";
  }

 bench_cpp(n, path, func, true);
}


return EXIT_SUCCESS;

}
