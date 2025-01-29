%% Example of using Ffilesystem Matlab class
% we haven't added all the Ffilesystem function as methods yet.
% However as seen in Ffilesystem.m, it is straightforward to add function methods.
%
% First build Ffilesystem library with CMake
%   cmake -Bbuildi -DBUILD_SHARED_LIBS=on
%   cmake --build buildi -t ffilesystem

% create an instance of Ffilesystem class
fs = Ffilesystem("../buildi");

% get CPU arch
cpu_arch = fs.cpu_arch();
disp(cpu_arch)

% get Ffilesystem backend
backend = fs.backend();
disp(backend)

% get Ffilesystem compiler
compiler = fs.compiler();
disp(compiler)

% close the Ffilesystem instance
% this unloads the C library
delete(fs);
