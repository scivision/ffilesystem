set_directory_properties(PROPERTIES LABELS "system")

set(boost_ut_tests memory)

if(HAVE_BOOST_UT)
foreach(t IN LISTS boost_ut_tests)
  add_executable(test_${t}_cpp test_${t}.cpp)
  target_link_libraries(test_${t}_cpp PRIVATE ffilesystem)
  target_compile_definitions(test_${t}_cpp PRIVATE
    $<$<NOT:$<BOOL:${ffilesystem_system}>>:FFS_SYSTEM_DISABLED>)
  target_include_directories(test_${t}_cpp PRIVATE ${boost_ut_SOURCE_DIR}/include)
  target_compile_options(test_${t}_cpp PRIVATE ${ffilesystem_cpp_test_flags})

  add_test(NAME ${t}_cpp COMMAND test_${t}_cpp)
  set_tests_properties(${t}_cpp PROPERTIES LABELS "Cpp")
endforeach()
endif()


set(fortran_tests memory)


if(ffilesystem_fortran)
foreach(t IN LISTS fortran_tests)

  add_executable(test_${t}_fortran test_${t}.f90)
  target_link_libraries(test_${t}_fortran PRIVATE ffilesystem)
  target_compile_options(test_${t}_fortran PRIVATE ${ffilesystem_fortran_test_flags})
  set_property(TARGET test_${t}_fortran PROPERTY LINKER_LANGUAGE ${ffilesystem_linker_lang})

  add_test(NAME Fortran_${t} COMMAND test_${t}_fortran
    $<$<NOT:$<BOOL:${ffilesystem_system}>>:disabled>)
  set_tests_properties(Fortran_${t} PROPERTIES
  LABELS "Fortran"
  )

endforeach()
endif()
