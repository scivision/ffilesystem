
foreach(t IN ITEMS is_symlink symlink)
  add_executable(test_${t}_cpp test_${t}.cpp)
  target_link_libraries(test_${t}_cpp PRIVATE ffilesystem)
  target_link_libraries(test_${t}_cpp PRIVATE GTest::gtest_main GTest::gmock)

  ffs_gtest_register(test_${t}_cpp
    PROPERTIES
      LABELS "Cpp"
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
endforeach()


if(ffilesystem_fortran)

foreach(t IN ITEMS is_symlink symlink)
  add_executable(test_${t}_fortran test_${t}.f90)
  target_link_libraries(test_${t}_fortran PRIVATE ffilesystem)
  target_compile_options(test_${t}_fortran PRIVATE ${ffilesystem_fortran_test_flags})
  set_property(TARGET test_${t}_fortran PROPERTY LINKER_LANGUAGE ${ffilesystem_linker_lang})
endforeach()

add_test(NAME Fortran_is_symlink COMMAND test_is_symlink_fortran)
set_tests_properties(Fortran_is_symlink PROPERTIES
FIXTURES_SETUP is_symlink
DISABLED $<BOOL:${sym_ret}>
)

add_test(NAME Fortran_symlink COMMAND test_symlink_fortran)
set_tests_properties(Fortran_symlink PROPERTIES
FIXTURES_SETUP symlink
FIXTURES_REQUIRED is_symlink
SKIP_RETURN_CODE 77
)

endif()
