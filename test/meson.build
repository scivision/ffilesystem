test_dirs = ['core', 'executable']
if get_option('extra')
  test_dirs += ['extra']
endif

foreach d : test_dirs
  subdir(d)
endforeach
