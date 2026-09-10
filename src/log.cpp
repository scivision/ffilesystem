#include "ffilesystem.h"

#include <atomic>
#include <iostream>
#include <sstream>

#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <cerrno>
#endif

namespace {

void fs_stderr_error_callback(const char* message)
{
  std::cerr << message;
}

std::atomic<fs_error_callback_t> fs_error_handler{fs_stderr_error_callback};

void fs_append_system_error(std::ostringstream& message)
{
#if defined(_WIN32) || defined(__CYGWIN__)
  if (DWORD error = GetLastError(); error)
    message << "GetLastError: " << std::system_category().message(error) << " ";
#else
  if (errno) {
    auto econd = std::generic_category().default_error_condition(errno);
    message << "errno: " << econd.message() << "\n";
  }
#endif
}

void fs_deliver_error(std::ostringstream& message)
{
  fs_append_system_error(message);
  if (auto callback = fs_error_handler.load())
    callback(message.str().c_str());
}

} // namespace

void fs_set_error_callback(fs_error_callback_t callback)
{
  fs_error_handler.store(callback);
}

void fs_reset_error_callback()
{
  fs_error_handler.store(fs_stderr_error_callback);
}

void fs_error_callback(std::string_view path
#if defined(__cpp_lib_source_location)
, const std::source_location& location) {
  const auto source = std::string(location.file_name()) + ":" + std::to_string(location.line());
  const std::string_view function = location.function_name();
#else
) {
  const std::string_view source;
  const std::string_view function;
#endif
  std::ostringstream message;
  message << "ERROR: Ffilesystem: " << function << "(" << path << ")\n" << source << "\n";
  fs_deliver_error(message);
}

void fs_error_callback(std::string_view path, const std::error_code& error
#if defined(__cpp_lib_source_location)
, const std::source_location& location) {
  const auto source = std::string(location.file_name()) + ":" + std::to_string(location.line());
  const std::string_view function = location.function_name();
#else
) {
  const std::string_view source;
  const std::string_view function;
#endif
  std::ostringstream message;
  message << "ERROR: Ffilesystem: " << function << "(" << path << ")\n" << source << "\n";
  if (error)
    message << error.message() << " " << error.value() << "\n";
  fs_deliver_error(message);
}

void fs_error_callback(std::string_view path1, std::string_view path2
#if defined(__cpp_lib_source_location)
, const std::source_location& location) {
  const auto source = std::string(location.file_name()) + ":" + std::to_string(location.line());
  const std::string_view function = location.function_name();
#else
) {
  const std::string_view source;
  const std::string_view function;
#endif
  std::ostringstream message;
  message << "ERROR: Ffilesystem: " << function << "(" << path1 << ", " << path2 << ")\n" << source << "\n";
  fs_deliver_error(message);
}

void fs_error_callback(std::string_view path1, std::string_view path2, const std::error_code& error
#if defined(__cpp_lib_source_location)
, const std::source_location& location) {
  const auto source = std::string(location.file_name()) + ":" + std::to_string(location.line());
  const std::string_view function = location.function_name();
#else
) {
  const std::string_view source;
  const std::string_view function;
#endif
  std::ostringstream message;
  message << "ERROR: Ffilesystem: " << function << "(" << path1 << ", " << path2 << ")\n" << source << "\n";
  if (error)
    message << "C++ exception: " << error.message() << " " << error.value() << "\n";
  fs_deliver_error(message);
}
