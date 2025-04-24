set_property(DIRECTORY PROPERTY LABELS "core")

if(HAVE_Fortran_FILESYSTEM)

set(tests is_absolute absolute
canonical realpath
env
equivalent
exe
expanduser
filename
filesize space
is_dir
is_empty
is_subdir join
normal parent root
resolve
relative proximate
reserved safe
set_cwd
separator
stem
suffix
with_suffix
owner
permissions
copyfile
mkdir
touch
which
)

if(HAVE_F03TYPE)
  list(APPEND tests path_t)
endif()

if(CYGWIN)
  list(APPEND tests cygwin windows)
elseif(WIN32)
  list(APPEND tests windows)
endif()

foreach(t IN LISTS tests)

add_executable(test_${t}_fortran test_${t}.f90)
target_link_libraries(test_${t}_fortran PRIVATE ffilesystem)
target_compile_options(test_${t}_fortran PRIVATE ${ffilesystem_fortran_test_flags})
set_property(TARGET test_${t}_fortran PROPERTY LINKER_LANGUAGE ${ffilesystem_linker_lang})

add_test(NAME Fortran_${t} COMMAND test_${t}_fortran)
set_tests_properties(Fortran_${t} PROPERTIES
LABELS Fortran
SKIP_RETURN_CODE 77
WORKING_DIRECTORY $<TARGET_FILE_DIR:test_${t}_fortran>
)

endforeach()

set_tests_properties(Fortran_relative Fortran_proximate PROPERTIES DISABLED $<NOT:$<BOOL:${HAVE_CXX_FILESYSTEM}>>)

endif()


set(gtest_tests
absolute
canonical
copyfile
drop_slash
empty
env
equivalent
exe
expanduser
filename
filesize
hard_link
is_absolute
is_char
OnDisk
is_empty
is_fifo
is_prefix
is_subdir
join
owner
normal
parent
permissions
remove
rename
reserved
root
safe
separator
set_cwd
space
stem
suffix
system
utf8
which
with_suffix
)

if(WIN32 OR CYGWIN)
  list(APPEND gtest_tests is_appexec_alias)
endif()
if(HAVE_CXX_FILESYSTEM)
  list(APPEND gtest_tests proximate relative)
endif()

foreach(t IN LISTS gtest_tests)
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
      WORKING_DIRECTORY $<TARGET_FILE_DIR:test_${t}_cpp>
  )
endforeach()

# C

foreach(t IN ITEMS overflow)

add_executable(test_${t}_c test_${t}.c)
target_link_libraries(test_${t}_c PRIVATE ffilesystem)

add_test(NAME C_${t} COMMAND test_${t}_c)
set_tests_properties(C_${t} PROPERTIES
LABELS "C"
WORKING_DIRECTORY $<TARGET_FILE_DIR:test_${t}_c>
)

endforeach()
