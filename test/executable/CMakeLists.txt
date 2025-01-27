foreach(t IN ITEMS exe which)

add_executable(test_${t}_cpp test_${t}.cpp)
target_link_libraries(test_${t}_cpp PRIVATE ffilesystem)
target_include_directories(test_${t}_cpp PRIVATE ..)

add_test(NAME Cpp_${t} COMMAND test_${t}_cpp)

set_tests_properties(Cpp_${t} PROPERTIES
LABELS "Cpp"
SKIP_RETURN_CODE 77
WORKING_DIRECTORY $<TARGET_FILE_DIR:test_${t}_cpp>
)

endforeach()

add_test(NAME Cpp_which_noPath COMMAND test_which_cpp 1)

set_tests_properties(Cpp_which_noPath PROPERTIES LABELS "Cpp")



if(HAVE_Fortran_FILESYSTEM)

foreach(t IN ITEMS exe which)

add_executable(test_${t}_fortran test_${t}.f90)
target_link_libraries(test_${t}_fortran PRIVATE ffilesystem)
target_compile_options(test_${t}_fortran PRIVATE ${ffilesystem_fortran_test_flags})
set_property(TARGET test_${t}_fortran PROPERTY LINKER_LANGUAGE ${ffilesystem_linker_lang})

add_test(NAME Fortran_${t} COMMAND test_${t}_fortran)

set_tests_properties(Fortran_${t} PROPERTIES
LABELS "Fortran"
SKIP_RETURN_CODE 77
WORKING_DIRECTORY $<TARGET_FILE_DIR:test_${t}_fortran>
)

endforeach()

endif()
