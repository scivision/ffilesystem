#include <string_view>
#include <string>
#include <cstdint>

#include <algorithm> // std::transform
#include <cctype>    // std::tolower

#include <iostream>

// include even if <filesystem> is available
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif defined(HAVE_CXX_FILESYSTEM)
#include <filesystem>
#else
#include <unistd.h>  // X_OK, access()
#endif

#if !defined(_WIN32)
#include <fstream>
#include <array>
#endif

#if defined(__APPLE__)
#include <cstring>   // std::memcpy
#include <mach-o/loader.h>
#include <mach-o/fat.h>
#endif

#include "ffilesystem.h"


bool fs_is_executable_binary(std::string_view path)
{
  // is path a binary executable file as determined by the magic number
  // beginning the file.  This is a heuristic and may not be 100% accurate.
  bool ok = false;
#if defined(_WIN32)
  // MinGW, MSVC, oneAPI at least need || is_appexec_alias()
  DWORD t;
  ok = (GetBinaryTypeA(path.data(), &t) != 0) || fs_is_appexec_alias(path);
#else
  // https://github.com/jart/cosmopolitan/blob/master/ape/specification.md

  std::array<char, 4> magic;

  if(std::ifstream f{path.data(), std::ios::binary}){
    if(f.read(magic.data(), 4).gcount() != 4)
      return false;
  } else
    return false;

#if defined(__APPLE__)
  // https://ss64.com/mac/file.html
    // Mach-O magic numbers
    // https://stackoverflow.com/q/27669766
    // https://jonasdevlieghere.com/post/macho-4gb-limit/
    // MH_MAGIC: 0xfeedface (32-bit)
    // MH_MAGIC_64: 0xfeedfacf (64-bit)
    // FAT_MAGIC: 0xCAFEBABE (32-bit)
    // FAT_MAGIC_64: 0xCAFEBABF (64-bit)
    // it's necessary to consider the reversed magic number for each of these as well,
    // done in the "*CIGAM*" constants

  std::uint32_t um32;
  std::memcpy(&um32, magic.data(), sizeof(um32));

  ok = um32 == MH_MAGIC ||  um32 == MH_MAGIC_64 ||
       um32 == MH_CIGAM ||  um32 == MH_CIGAM_64 ||
       um32 == FAT_MAGIC || um32 == FAT_MAGIC_64 ||
       um32 == FAT_CIGAM || um32 == FAT_CIGAM_64;

#else
  // Linux / BSD: executable binary check via ELF file magic number
    ok = magic[0] == 0x7f && magic[1] == 'E' && magic[2] == 'L' && magic[3] == 'F';
  // does not consider PE or COFF formats.
#endif
#endif

  return ok;
}


bool fs_is_exe(std::string_view path)
{
  // is path (file or symlink to a file) executable by the user
  // directories are not considered executable--use fs_get_permissions() for that.

  if(!fs_is_file(path))
    return false;

#if defined(_WIN32)
  std::string suffix = fs_suffix(path);
  if (suffix.empty())
    return fs_is_executable_binary(path);

  if(!fs_is_readable(path))
    return false;

  std::string pathext = fs_getenv("PATHEXT");
  if(pathext.empty())
    pathext = ".com;.exe;.bat;.cmd";
  else
    std::transform(pathext.begin(), pathext.end(), pathext.begin(), ::tolower);

  std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);
  if(fs_trace) std::cout << "TRACE: is_exe(" << path << "):  suffix: " << suffix << " length " << suffix.length() << " pathext: " << pathext << "\n";

#if defined(__cpp_lib_string_contains) // C++23
  return pathext.contains(suffix);
#else
  return pathext.find(suffix) != std::string::npos;
#endif

#elif defined(HAVE_CXX_FILESYSTEM)

#if defined(__cpp_using_enum)  // C++20
  using enum std::filesystem::perms;
#else
  constexpr std::filesystem::perms none = std::filesystem::perms::none;
  constexpr std::filesystem::perms others_exec = std::filesystem::perms::others_exec;
  constexpr std::filesystem::perms group_exec = std::filesystem::perms::group_exec;
  constexpr std::filesystem::perms owner_exec = std::filesystem::perms::owner_exec;
#endif

  std::error_code ec;
  const auto s = std::filesystem::status(path, ec);
  if(ec){
    fs_print_error(path, __func__, ec);
    return false;
  }

  return (s.permissions() & (owner_exec | group_exec | others_exec)) != none;

#else
  return access(path.data(), X_OK) == 0;
#endif
}
