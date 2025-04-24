if(NOT BUILD_SHARED_LIBS)
  return()
endif()

if(CYGWIN)
  set(_p $<PATH:GET_FILENAME,ffilesystem>)
else()
  set(_p $<TARGET_FILE:ffilesystem>)
endif()

foreach(t IN ITEMS libpath)
  add_executable(test_${t}_cpp test_${t}.cpp)
  target_link_libraries(test_${t}_cpp PRIVATE ffilesystem)
  if(CMAKE_CXX_STANDARD LESS 20 AND "cxx_std_20" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    # enable source_location
    target_compile_features(test_${t}_cpp PRIVATE cxx_std_20)
  endif()

  target_link_libraries(test_${t}_cpp PRIVATE GTest::gtest_main GTest::gmock)
  gtest_discover_tests(test_${t}_cpp
    PROPERTIES
      LABELS "Cpp"
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      PASS_REGULAR_EXPRESSION "${_p}"
  )
endforeach()



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
