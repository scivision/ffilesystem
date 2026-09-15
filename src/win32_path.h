#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>
#include <string_view>
#include <optional>
#endif

#if defined(_WIN32)

struct fs_win32_path_parts {
  std::string drive;
  std::string directory;
};

std::optional<fs_win32_path_parts> fs_win32_split_path(std::string_view path);

bool fs_win32_is_ext_path(std::string_view path);

#endif

#if defined(_WIN32) || defined(__CYGWIN__)
bool fs_win32_is_symlink(std::string_view path);

std::string fs_win32_final_path(std::string_view);
std::string fs_win32_full_name(std::string_view);
HANDLE fs_win32_get_file_handle(std::string_view);
std::string fs_win32_to_narrow(std::wstring_view);
std::wstring fs_win32_to_wide(std::string_view);
#endif
