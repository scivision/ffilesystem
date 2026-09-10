#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ffilesystem.h"

static int callback_count;
static char message[256];

static void capture_error(const char* diagnostic)
{
  callback_count++;
  snprintf(message, sizeof(message), "%s", diagnostic);
}

int main(int argc, char* argv[])
{
  if (argc != 2 || (strcmp(argv[1], "0") != 0 && strcmp(argv[1], "1") != 0))
    return EXIT_FAILURE;

  if (strcmp(argv[1], "0") == 0) {
    fs_set_error_callback(capture_error);
    if (fs_file_size("test-path") != UINTMAX_MAX){
      fprintf(stderr, "Unexpected file size for test-path\n");
      return EXIT_FAILURE;
    }

    if (callback_count != 1 || strstr(message, "ERROR: Ffilesystem:") == NULL ||
        strstr(message, "test-path") == NULL){
      fprintf(stderr, "Unexpected error message: %s\n", message);
      return EXIT_FAILURE;
    }

    message[0] = '\0';
    fs_set_error_callback(NULL);
    if (fs_file_size("test-path") != UINTMAX_MAX || callback_count != 1 || message[0] != '\0') {
      fprintf(stderr, "Unexpected error message after resetting callback: %s\n", message);
      return EXIT_FAILURE;
    }
  } else {
    fs_reset_error_callback();
    if (fs_file_size("test-path") != UINTMAX_MAX) {
      fprintf(stderr, "Unexpected file size for test-path\n");
      return EXIT_FAILURE;
    }
  }

  fs_reset_error_callback();
  return EXIT_SUCCESS;
}
