option(ffilesystem_cpp "Use C++ filesystem for full functionality" on)
option(ffilesystem_fortran "use the Fortran interaces to C functions" on)
option(ffilesystem_cli "Build CLI" ${ffilesystem_IS_TOP_LEVEL})
option(ffilesystem_fallback "Fallback to non-C++ if C++ stdlib is not working" on)
option(ffilesystem_trace "debug trace output" off)
option(ffilesystem_locale "enable locale-based functions")
option(ffilesystem_extra "enable extra functions not strictly filesystem-based" on)
option(ffilesystem_unicode "Windows Unicode support" on)

option(ffilesystem_statx "use statx() if available" on)

option(BUILD_SHARED_LIBS "Build shared libraries")
option(ffilesystem_coverage "Code coverage tests")
option(ffilesystem_tidy "Run clang-tidy on the code")
option(ffilesystem_cppcheck "Run cppcheck on the code")
option(ffilesystem_iwyu "Run include-what-you-use on the code")

option(ffilesystem_BUILD_TESTING "Build tests" ${ffilesystem_IS_TOP_LEVEL})

file(GENERATE OUTPUT .gitignore CONTENT "*")
