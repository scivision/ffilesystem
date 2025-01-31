set_directory_properties(PROPERTIES LABELS "extra")

set(extra_tests_cpp exepath filesystem_type hostname max_component shell terminal)

if(ffilesystem_locale)
  list(APPEND extra_tests_cpp locale)
endif()

foreach(t IN LISTS extra_tests_cpp)

  add_executable(test_${t}_cpp test_${t}.cpp)
  target_link_libraries(test_${t}_cpp PRIVATE ffilesystem)
  target_include_directories(test_${t}_cpp PRIVATE ..)
  if(CMAKE_CXX_STANDARD LESS 20 AND "cxx_std_20" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    target_compile_features(test_${t}_cpp PRIVATE cxx_std_20)
  endif()

  add_test(NAME Cpp_${t} COMMAND ${CMAKE_COMMAND} -E env $<TARGET_FILE:test_${t}_cpp>)
  set_tests_properties(Cpp_${t} PROPERTIES
  LABELS "Cpp"
  SKIP_RETURN_CODE 77
  WORKING_DIRECTORY $<TARGET_FILE_DIR:test_${t}_cpp>
  FIXTURES_SETUP ${t}
  )

endforeach()


if(TEST Cpp_locale)

set_tests_properties(Cpp_locale PROPERTIES
SKIP_REGULAR_EXPRESSION "locale\:\:facet\:\:\_S\_create\_c\_locale name not valid"
)

endif()

# TARGET_FILE_NAME to allow for macOS possible non-canonical result
# and symlinked / special filesystems like WSL
set_tests_properties(Cpp_exepath PROPERTIES PASS_REGULAR_EXPRESSION $<TARGET_FILE_NAME:test_exepath_cpp>)


# --------- Fortran


if(HAVE_Fortran_FILESYSTEM)

set(extra_tests_fortran exepath filesystem_type)


foreach(t IN LISTS extra_tests_fortran)

  add_executable(test_${t}_fortran test_${t}.f90)
  target_link_libraries(test_${t}_fortran PRIVATE ffilesystem)
  target_compile_options(test_${t}_fortran PRIVATE ${ffilesystem_fortran_test_flags})
  set_property(TARGET test_${t}_fortran PROPERTY LINKER_LANGUAGE ${ffilesystem_linker_lang})

  add_test(NAME Fortran_${t} COMMAND test_${t}_fortran)
  set_tests_properties(Fortran_${t} PROPERTIES
  LABELS Fortran
  SKIP_RETURN_CODE 77
  WORKING_DIRECTORY $<TARGET_FILE_DIR:test_${t}_fortran>
  FIXTURES_SETUP ${t}
  )

endforeach()

target_compile_options(test_filesystem_type_fortran PRIVATE
$<$<COMPILE_LANG_AND_ID:Fortran,GNU>:-Wno-uninitialized>
)

set_tests_properties(Fortran_filesystem_type PROPERTIES
DISABLED $<AND:$<BOOL:${LINUX}>,$<NOT:$<BOOL:${ffilesystem_HAVE_LINUX_MAGIC}>>>
)

# TARGET_FILE_NAME to allow for macOS possible non-canonical result
# and symlinked / special filesystems like WSL
set_tests_properties(Fortran_exepath PROPERTIES PASS_REGULAR_EXPRESSION $<TARGET_FILE_NAME:test_exepath_fortran>)



endif()
