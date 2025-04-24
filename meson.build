project('ffilesystem', ['c', 'cpp'],
version: '6.3.2',
default_options: ['cpp_std=c++23,c++20,c++17', 'buildtype=release', 'b_ndebug=if-release'],
meson_version : '>=1.4.0')

backend_filesystem = get_option('cpp')
# use C++17 <filesystem> library

trace_def = get_option('trace') ? '-DFS_TRACE' : []

have_filesystem = backend_filesystem ? '-DHAVE_CXX_FILESYSTEM' : []

fortran = get_option('fortran')
if fortran
  add_languages('fortran', native: false, required: true)
endif

cpp_std = get_option('cpp_std')
message('C++ standard: ' + cpp_std)

cc = meson.get_compiler('c')
cpp = meson.get_compiler('cpp')

if ['gcc', 'clang', 'intel-llvm'].contains(cc.get_id())
  add_project_arguments('-Werror=implicit-function-declaration', language: 'c')
endif

fsrcs = fortran ? files('src/fortran/filesystem.F90') : []

comm_src = files(
'src/c.cpp',
'src/absolute.cpp',
'src/extra/compiler.cpp',
'src/copy.cpp',
'src/extra/cygwin.cpp',
'src/equivalent.cpp',
'src/env.cpp',
'src/executable.cpp',
'src/home.cpp',
'src/inquire.cpp',
'src/lang.cpp',
'src/lexical.cpp',
'src/limits.cpp',
'src/extra/locale.cpp',
'src/log.cpp',
'src/mkdir.cpp',
'src/move.cpp',
'src/normalize.cpp',
'src/extra/owner.cpp',
'src/os.c',
'src/parent.cpp',
'src/partition.cpp',
'src/permissions.cpp',
'src/platform.cpp',
'src/pure.cpp',
'src/relative.cpp',
'src/resolve.cpp',
'src/size.cpp',
'src/space.cpp',
'src/symlink.cpp',
'src/tempdir.cpp',
'src/time.cpp',
'src/touch.cpp',
'src/which.cpp',
'src/windows.cpp'
)

if get_option('extra')
  comm_src += files(
'src/extra/case.cpp',
'src/extra/component.cpp',
'src/extra/exepath.cpp',
'src/extra/libpath.cpp',
'src/extra/random.cpp',
'src/extra/shell.cpp',
'src/extra/sysctl.cpp',
'src/extra/uid.cpp',
'src/extra/uname.cpp',
'src/extra/winsock.cpp',
)
endif

extra_def = get_option('extra') ? '-Dffilesystem_extra' : []
locale_def = get_option('locale') ? '-Dffilesystem_locale' : []

inc = include_directories('include')

is_windows = host_machine.system() == 'windows'

# -- dladdr check
dladdr_def = []
dl = []

if not is_windows and get_option('default_library') == 'shared'
  dl = cpp.find_library('dl', required: false)

  if cpp.has_function('dladdr', args: '-D_GNU_SOURCE', dependencies: dl, prefix: '#include <dlfcn.h>')
    dladdr_def = '-DHAVE_DLADDR'
  endif
endif

# -- C++ Feature checks
chrono_def = []

fs_lib = []
def_copy_file = []

if backend_filesystem

if cpp.get_id() != 'msvc'
  r = cpp.run(files('cmake/libstdcpp_version.cpp'))
  if r.returncode() == 0
    stdcpp_version = r.stdout().strip()
    message('C++ STL: ' + stdcpp_version)
    if stdcpp_version.startswith('GNU') and stdcpp_version.split()[1].version_compare('<9')
      fs_lib = [cpp.find_library('stdc++fs'), cpp.find_library('stdc++')]
    endif
  else
    warning('Failed to determine C++ STL version')
  endif
endif

fs_check = '''
#include <filesystem>
int main () {
std::filesystem::path tgt("a/b");
return !tgt.has_filename();
}'''

r = cpp.links(fs_check, dependencies: fs_lib, name: 'C++ filesystem')
if not r
  error('No C++ <filesystem> support')
endif

# -- chrono check
if cpp_std.version_compare('>=20')
  chrono_src = '''
  #include <chrono>
  #include <filesystem>

  int main(){

  std::filesystem::file_time_type t_fs;
  auto t_sys = std::chrono::clock_cast<std::chrono::system_clock>(t_fs);
  return 0;
  }
  '''
  if cpp.links(chrono_src, dependencies: fs_lib, name: 'C++20 chrono')
    chrono_def = '-DHAVE_CLOCK_CAST'
  endif
endif

else
# use libc instead of <filesystem>

# -- copy_file_range()

if not is_windows
  if cpp.has_function('copy_file_range', args: '-D_GNU_SOURCE', prefix: '#include <unistd.h>')
    def_copy_file = '-DHAVE_COPY_FILE_RANGE'
  endif
endif

endif

# Windows-specific
libwin = []

if is_windows

libwin = [cc.find_library('Secur32'), cc.find_library('Userenv'), cc.find_library('ws2_32')]

endif

def_statx = get_option('statx') ? '-DUSE_STATX' : []

# -- main library

filesystem = library('filesystem',
sources: [fsrcs, comm_src],
include_directories: [inc],
install: true,
c_args: [],
cpp_args: [have_filesystem, chrono_def, dladdr_def,
def_copy_file, def_statx, extra_def, locale_def, trace_def],
dependencies: [libwin, fs_lib, dl],
)

if not meson.is_subproject()

executable('libc_version', 'app/libc.cpp')
executable('libcpp_version', 'app/libcxx.cpp')

executable('fs_cli', sources: 'app/main.cpp', link_with: filesystem, include_directories: inc, install: true)

if fortran

executable('filesystem_cli',
  sources: 'app/fortran/main.f90',
  link_with: filesystem,
  install: true)

endif

endif

if get_option('test')
  gtest_proj = subproject('gtest')
  gtest_dep = gtest_proj.get_variable('gtest_main_dep')
  gmock_dep = gtest_proj.get_variable('gmock_dep')
  if not gtest_dep.found()
    error('MESON_SKIP_TEST: gtest not installed.')
  endif
  if not gmock_dep.found()
    error('MESON_SKIP_TEST: gmock not installed.')
  endif

  subdir('test')

  executable('ffilesystem_bench', 'app/benchmark.cpp', link_with: filesystem, include_directories: inc, install: true)
endif
