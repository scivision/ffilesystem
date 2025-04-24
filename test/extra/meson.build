cpp_extra = ['hostname']

if get_option('locale')
  cpp_extra += ['locale']
endif

foreach t : cpp_extra

  t_cpp = executable(t + '_cpp', 'test_' + t + '.cpp',
    link_with: filesystem,
    dependencies: [gtest_dep, gmock_dep],
    include_directories: inc)

  test(t + '_cpp', t_cpp, suite: ['cpp', 'extra'], workdir: meson.current_build_dir())

endforeach



if fortran

fortran_extra = [
'exepath',
'filesystem_type',
]


foreach t : fortran_extra

tec = executable(t + '_fortran',
    'test_' + t + '.f90',
            link_with: filesystem, include_directories: inc)

test(t + '_fortran', tec,
      suite: ['fortran','extra'],
      workdir: meson.current_build_dir())

endforeach

endif
