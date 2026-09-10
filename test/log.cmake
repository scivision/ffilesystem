cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED exe OR NOT DEFINED mode)
  message(FATAL_ERROR "Specify -Dexe=<executable> and -Dmode=<0|1>")
endif()

execute_process(
  COMMAND ${exe} ${mode}
  RESULT_VARIABLE result
  COMMAND_ECHO STDOUT
  OUTPUT_VARIABLE out
  ERROR_VARIABLE err
)

if(NOT result EQUAL 0)
  message(FATAL_ERROR "unexpected return code: ${result}
  ${err}
  ${out}")
endif()

if(NOT out STREQUAL "")
  message(FATAL_ERROR "unexpected stdout output:\n${out}")
endif()

if(mode STREQUAL "0")
  if(NOT err STREQUAL "")
    message(FATAL_ERROR "unexpected stderr output:\n${err}")
  endif()
elseif(mode STREQUAL "1")
  if(NOT err MATCHES "ERROR: Ffilesystem:")
    message(FATAL_ERROR "expected Ffilesystem diagnostic on stderr:\n${err}")
  endif()
else()
  message(FATAL_ERROR "mode must be 0 or 1")
endif()
