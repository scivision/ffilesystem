cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED exe)
  message(FATAL_ERROR "Please specify the executable to run -Dexe=<exe>")
endif()

execute_process(COMMAND ${exe}
INPUT_FILE ${CMAKE_CURRENT_LIST_DIR}/cli_exercise.txt
COMMAND_ERROR_IS_FATAL ANY
COMMAND_ECHO STDOUT
OUTPUT_VARIABLE out
ERROR_VARIABLE err
)

if(out MATCHES "TRACE")
  message(FATAL_ERROR "TRACE found in stdout")
endif()

if(err MATCHES "TRACE")
  message(FATAL_ERROR "TRACE found in stderr")
endif()

if(DEFINED ffilesystem_system AND NOT ffilesystem_system)
  string(REPLACE "\r\n" "\n" err "${err}")
  set(remaining "${err}")
  set(disabled_diagnostic
    "ERROR: Ffilesystem: ([^\n]*fs_(get_username|cpu_arch|get_shell|hostname|is_admin)\\(\\)\\(\\)\n[^\n]*[/\\\\]src[/\\\\]sys[/\\\\]disabled.cpp:[0-9]+|\\(\\)\n)\n[^\n]+ [0-9]+\n")
  string(REGEX REPLACE "${disabled_diagnostic}" "" remaining "${remaining}")
  string(STRIP "${remaining}" remaining)
  if(NOT err MATCHES "${disabled_diagnostic}" OR NOT remaining STREQUAL "")
    message(FATAL_ERROR "Unexpected or missing disabled-system diagnostics:\n${err}")
  endif()
  set(err "")
endif()

if(NOT err STREQUAL "")
  message(FATAL_ERROR "stderr output is not empty:
  ${err}")
endif()

message(STATUS "stdout:
${out}")
