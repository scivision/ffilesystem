#include "ffilesystem.h"

#include <cstdlib>
#include <string>
#include <iostream>
namespace {

std::string received_message;
int callback_count;

void capture_error(const char* message)
{
  callback_count++;
  received_message = message;
}

} // namespace

int main(int argc, char* argv[]) {
  if (argc != 2 || (std::string_view(argv[1]) != "0" && std::string_view(argv[1]) != "1")) {
    std::cerr << "Usage: " << argv[0] << " <0|1>\n";
    return EXIT_FAILURE;
  }

  if (std::string_view(argv[1]) == "0") {
    fs_set_error_callback(capture_error);
    fs_error_callback("test-path", std::make_error_code(std::errc::io_error));

    if (callback_count != 1 || received_message.find("ERROR: Ffilesystem:") == std::string::npos ||
        received_message.find("test-path") == std::string::npos) {
          std::cerr << "Unexpected error message: " << received_message << "\n";
          return EXIT_FAILURE;
        }

    received_message.clear();
    fs_set_error_callback(nullptr);
    fs_error_callback("test-path");

    if (callback_count != 1 || !received_message.empty()) {
      std::cerr << "Unexpected error message after resetting callback: " << received_message << "\n";
      return EXIT_FAILURE;
    }
  } else {
    fs_reset_error_callback();
    fs_error_callback("test-path", std::make_error_code(std::errc::io_error));
  }

  fs_reset_error_callback();
  return EXIT_SUCCESS;
}
