set_property(DIRECTORY PROPERTY LABELS "core")

if(HAVE_Fortran_FILESYSTEM)

set(tests is_absolute absolute
canonical realpath
env
equivalent
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
copyfile mkdir touch
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


set(cpp_tests
drop_slash
permissions
)

if(WIN32)
  list(APPEND cpp_tests is_appexec_alias)
endif()

set(gtest_tests
absolute
canonical
copyfile
empty
env
equivalent
expanduser
filename
filesize
hard_link
is_absolute
is_char
is_dir
is_empty
is_fifo
is_file
is_prefix
is_subdir
join
mkdir
owner
normal
parent
proximate
realpath
relative
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
touch
with_suffix
)
if(WIN32 OR CYGWIN)
  list(APPEND gtest_tests windows)
endif()

foreach(t IN LISTS cpp_tests gtest_tests)
  add_executable(test_${t}_cpp test_${t}.cpp)
  target_link_libraries(test_${t}_cpp PRIVATE ffilesystem)
  target_include_directories(test_${t}_cpp PRIVATE ..)

  if(t IN_LIST gtest_tests)
    target_link_libraries(test_${t}_cpp PRIVATE GTest::gtest_main GTest::gmock)
    gtest_add_tests(TARGET test_${t}_cpp)
    continue()
  endif()

  if(CMAKE_CXX_STANDARD LESS 20 AND "cxx_std_20" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    # enable source_location
    target_compile_features(test_${t}_cpp PRIVATE cxx_std_20)
  endif()

  add_test(NAME Cpp_${t} COMMAND test_${t}_cpp)
  set_tests_properties(Cpp_${t} PROPERTIES
  LABELS "Cpp"
  SKIP_RETURN_CODE 77
  WORKING_DIRECTORY $<TARGET_FILE_DIR:test_${t}_cpp>
  FIXTURES_SETUP ${t}
  )
endforeach()

target_compile_definitions(test_touch_cpp PRIVATE $<$<BOOL:${MSVC}>:_CRT_SECURE_NO_WARNINGS>)

set_tests_properties(TestRelative.Relative TestProximate.Proximate PROPERTIES DISABLED $<NOT:$<BOOL:${HAVE_CXX_FILESYSTEM}>>)

if(TARGET test_env_fortran)
  add_test(NAME Fortran_NoEnv COMMAND test_env_fortran)
  set(_noenv Fortran_NoEnv)
else()
  set(_noenv)
endif()
add_test(NAME Cpp_NoEnv COMMAND test_env_cpp)

if(CMAKE_VERSION VERSION_LESS 3.22 OR "$ENV{CI}")
  set_tests_properties(Cpp_NoEnv ${_noenv} PROPERTIES DISABLED true)
else()
  set_tests_properties(Cpp_NoEnv ${_noenv} PROPERTIES ENVIRONMENT_MODIFICATION "HOME=unset:;USERPROFILE=unset:;TEMP=unset:;TMPDIR=unset:")
endif()

# C

foreach(t IN ITEMS overflow)

add_executable(test_${t}_c test_${t}.c)
target_link_libraries(test_${t}_c PRIVATE ffilesystem)

add_test(NAME C_${t} COMMAND test_${t}_c)
set_tests_properties(C_${t} PROPERTIES
LABELS "C"
SKIP_RETURN_CODE 77
WORKING_DIRECTORY $<TARGET_FILE_DIR:test_${t}_c>
FIXTURES_SETUP ${t}
)

endforeach()
