#pragma once

#if defined(_WIN32)

#include <string>
#include <string_view>
#include <optional>

struct fs_win32_path_parts {
  std::string drive;
  std::string directory;
};

std::optional<fs_win32_path_parts> fs_win32_split_path(std::string_view path);

#endif
