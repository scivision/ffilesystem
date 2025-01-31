if(BUILD_SHARED_LIBS)
  if(CYGWIN)
    set(_p $<PATH:GET_FILENAME,ffilesystem>)
  else()
    set(_p $<TARGET_FILE:ffilesystem>)
  endif()
  set(_dis)
else()
  set(_dis DISABLED true)
endif()

add_executable(test_libpath_cpp test_libpath.cpp)
target_link_libraries(test_libpath_cpp PRIVATE ffilesystem)
target_include_directories(test_libpath_cpp PRIVATE ..)
if(CMAKE_CXX_STANDARD LESS 20 AND "cxx_std_20" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
  target_compile_features(test_libpath_cpp PRIVATE cxx_std_20)
endif()

add_test(NAME Cpp_libpath COMMAND test_libpath_cpp)

set_tests_properties(Cpp_libpath PROPERTIES
PASS_REGULAR_EXPRESSION "${_p}"
LABELS "Cpp"
${_dis}
)


if(HAVE_Fortran_FILESYSTEM)

add_executable(test_libpath_fortran test_libpath.f90)
target_link_libraries(test_libpath_fortran PRIVATE ffilesystem)
target_compile_options(test_libpath_fortran PRIVATE ${ffilesystem_fortran_test_flags})
set_property(TARGET test_libpath_fortran PROPERTY LINKER_LANGUAGE ${ffilesystem_linker_lang})

add_test(NAME Fortran_libpath COMMAND test_libpath_fortran)

set_tests_properties(Fortran_libpath PROPERTIES
PASS_REGULAR_EXPRESSION "${_p}"
LABELS "Fortran"
${_dis}
)

endif()
