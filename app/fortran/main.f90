program repl

use, intrinsic :: iso_fortran_env, only: stdout=>output_unit, stderr=>error_unit, stdin=>input_unit

use filesystem

implicit none

!! make a parser for space delimited input
integer, parameter :: Lcmd = 40

character(1000) :: inp
character(Lcmd) :: cmd
character(1000) :: arg1, arg2

integer :: i, i0, i1
logical :: ok, done

character, parameter :: delim = " "

! avoid garbage init values
arg1 = ""
arg2 = ""

if(is_admin()) write(stderr, '(a)') "WARNING: running as admin / sudo"

print '(a)', "Backend: " // backend()

main : do
  write(stdout, "(a)", advance="no") "Ffs> "

  read(stdin, '(A)', iostat=i) inp
  if (i /= 0) exit
  if (is_iostat_end(i)) exit
  if (len_trim(inp) == 1 .and. (inp(1:1) == "q" .or. iachar(inp(1:1)) == 4)) exit
  if (len_trim(inp) == 0) cycle


  i1 = index(inp, delim)
  if (i1 == 0) then
    cmd = inp(:Lcmd)
  else
    cmd = inp(:i1-1)
  end if

  done = .true.
  select case (cmd)
  case ("optimized")
    print '(L1)', fs_is_optimized()
  case ("backend")
    print '(a)', backend()
  case ('pid')
    print '(i0)', fs_getpid()
  case ("maxpath")
    print '(i0)', max_path()
  case ("cpp_lang")
    print '(i0)', cpp_lang()
  case ("c_lang")
    print '(i0)', c_lang()
  case ("pathsep")
    print '(A)', pathsep()
  case ("compiler")
    print '(A)', compiler()
  case ("compiler_c")
    print '(A)', compiler_c()
  case ("shell")
    print '(A)', get_shell()
  case ("terminal")
    print '(A)', get_terminal()
  case ("cpu_arch")
    print '(A)', cpu_arch()
  case ("homedir")
    print '(A)', get_homedir()
  case ("cwd")
    print '(A)', get_cwd()
  case ("tempdir")
    print '(A)', get_tempdir()
  case ("is_admin")
    print '(L1)', is_admin()
  case ("is_bsd")
    print '(L1)', is_bsd()
  case ("is_linux")
    print '(L1)', is_linux()
  case ("is_macos")
    print '(L1)', is_macos()
  case ("is_rosetta")
    print '(L1)', is_rosetta()
  case ("is_unix")
    print '(L1)', is_unix()
  case ("is_windows")
    print '(L1)', is_windows()
  case ("is_wsl")
    print '(L1)', is_wsl()
  case ("is_mingw")
    print '(L1)', is_mingw()
  case ("is_cygwin")
    print '(L1)', is_cygwin()
  case ("exe_path")
    print '(A)', exe_path()
  case("lib_path")
    print '(A)', lib_path()
  case default
    done = .false.
  end select

  if (done) cycle

  i0 = i1 + 1

  i1 = i0 + index(inp(i0:), delim)
  if(i1 /= 0) arg1 = inp(i0:i1-1)
  i0 = i1

  i1 = i0 + index(inp(i0:), delim)
  if(i1 /= 0) arg2 = inp(i0:i1-1)


  done = .true.
  select case (cmd)
  case ("max_component")
    print '(i0)', max_component(arg1)
  case ("is_empty")
    print '(L1)', is_empty(arg1)
  case ("hard")
    print '(i0)', hard_link_count(arg1)
  case ("as_posix")
    print '(a)', as_posix(arg1)
  case ('modtime')
    print '(i0)', get_modtime(arg1)
  case ("expanduser")
    print '(A)', expanduser(arg1)
  case ("owner")
    print '(A,1x,A)', get_owner_name(arg1), get_owner_group(arg1)
  case ("which")
    print '(A)', which(arg1, arg2, .true.)
  case ("canonical")
    print '(A)', canonical(arg1, .true., .false.)
  case ("weakly_canonical")
    print '(A)', canonical(arg1, .false., .false.)
  case ("resolve")
    print '(A)', resolve(arg1, .false., .false.)
  case ("remove")
    call remove(arg1, ok)
  case ("normal")
    print '(A)', normal(arg1)
  case ("parent")
    print '(A)', parent(arg1)
  case ("root")
    print '(A)', root(arg1)
  case ("root_name")
    print '(A)', root_name(arg1)
  case ("stem")
    print '(A)', stem(arg1)
  case ("suffix")
    print '(A)', suffix(arg1)
  case ("filename")
    print '(A)', file_name(arg1)
  case ("is_absolute")
    print '(L1)', is_absolute(arg1)
  case ("is_appexec")
    print '(L1)', is_appexec_alias(arg1)
  case ("exists")
    print '(L1)', exists(arg1)
  case ("is_case_sensitive")
    print '(L1)', is_case_sensitive(arg1)
  case ("is_dir")
    print '(L1)', is_dir(arg1)
  case ("is_char")
    print '(L1)', is_char_device(arg1)
  case ("is_fifo")
    print '(L1)', is_fifo(arg1)
  case ("is_file")
    print '(L1)', is_file(arg1)
  case ("is_exe")
    print '(L1)', is_exe(arg1)
  case ("is_exe_bin")
    print '(L1)', is_executable_binary(arg1)
  case ("is_reserved")
    print '(L1)', is_reserved(arg1)
  case ("is_readable")
    print '(L1)', is_readable(arg1)
  case ("is_writable")
    print '(L1)', is_writable(arg1)
  case ("perm")
    print '(A)', get_permissions(arg1)
  case ("is_symlink")
    print '(L1)', is_symlink(arg1)
  case ('realpath')
    print '(A)', realpath(arg1)
  case ("read_symlink")
    print '(A)', read_symlink(arg1)
  case ("size")
    print '(i0)', file_size(arg1)
  case ("space_available")
    print '(i0)', space_available(arg1)
  case ("space_capacity")
    print '(i0)', space_capacity(arg1)
  case ("hostname")
    print '(A)', hostname()
  case ("username")
    print '(A)', get_username()
  case ("mkdir")
    call mkdir(arg1, ok)
    if (ok) then
      print '(a)', "created directory " // trim(arg1)
    else
      write(stderr, "(a)") "ERROR: failed to create directory " // trim(arg1)
    end if
  case ("chdir")
    print '(l1)', set_cwd(arg1)
  case ("longname")
    print '(A)', longname(arg1)
  case ("shortname")
    print '(A)', shortname(arg1)
  case ("getenv")
    print '(A)', getenv(arg1)
  case ("type")
    print '(A)', filesystem_type(arg1)
  case default
    done = .false.
  end select

  if (done) cycle


  done = .true.
  select case (cmd)

  case ("absolute")
    print '(A)', absolute(arg1, arg2, .true.)
  case ("rename")
    call fs_rename(arg1, arg2, ok)
  case ("with_suffix")
    print '(A)', with_suffix(arg1, arg2)
  case ("is_prefix")
    print '(L1)', is_prefix(arg1, arg2)
  case ("is_subdir")
    print '(L1)', is_subdir(arg1, arg2)
  case ("setenv")
    call setenv(arg1, arg2, ok)
    if (ok) then
      print '(a)', "set environment variable " // trim(arg1) // " = " // trim(arg2)
    else
      write(stderr, "(a)") "ERROR: failed to set environment variable " // trim(arg1) // " = " // trim(arg2)
    end if
  case ("join")
    print '(A)', join(arg1, arg2)
  case ("relative")
    print '(A)', relative_to(arg1, arg2)
  case ("proximate")
    print '(a)', proximate_to(arg1, arg2)
  case ("same")
    print '(L1)', same_file(arg1, arg2)
  case ("create_symlink")
    call create_symlink(arg1, arg2, ok)
    if (ok) then
      print '(a)', "created symlink " // trim(arg1) // " -> " // trim(arg2)
    else
      write(stderr, "(a)") "ERROR: failed to create symlink " // trim(arg1) // " -> " // trim(arg2)
    end if
  case ("copy")
    print '(a)', "copying " // trim(arg1) // " -> " // trim(arg2)
    call copy_file(arg1, arg2, ok=ok)
    if (.not. ok) write(stderr, "(a)") "ERROR: failed to copy " // trim(arg1) // " -> " // trim(arg2)
  case default
    done = .false.
  end select

  if (done) cycle

  write(stderr, "(a)") "unknown command: " // trim(cmd)
end do main

end program repl
