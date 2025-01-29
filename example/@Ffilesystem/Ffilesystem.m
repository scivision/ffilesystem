%% uses "ffilesystem.dll" shared library created by Matlab-compatible compiler
% for example on Windows, MSVC or Intel oneAPI

classdef Ffilesystem < handle
  properties
    functions
    lib
    max_path
  end

  methods
    function obj = Ffilesystem(libpath)
      arguments
        libpath (1,1) string = ""
      end
      obj.lib = obj.load_clib(libpath);
      obj.max_path = calllib(obj.lib, "fs_get_max_path");
      obj.functions = libfunctions(obj.lib);
    end

    % https://www.mathworks.com/help/matlab/matlab_oop/handle-class-destructors.html
    function delete(obj)
      if ~isempty(obj.lib)
        unloadlibrary(obj.lib);
      end
    end

    function pid = getpid(obj)
      pid = calllib(obj.lib, 'fs_getpid');
    end

    function o = is_optimized(obj)
      o = calllib(obj.lib, 'fs_is_optimized');
    end

    function t = backend(obj)
      t = obj.string_call("fs_backend");
    end

    function t = compiler(obj)
      t = obj.string_call("fs_compiler");
    end

    function t = cpu_arch(obj)
      t = obj.string_call("fs_cpu_arch");
    end

    function t = exe_path(obj)
      t = obj.string_call("fs_exe_path");
    end

    function t = lib_path(obj)
      t = obj.string_call("fs_lib_path");
    end

    function t = shell(obj)
      t = obj.string_call("fs_get_shell");
    end

    function t = terminal(obj)
      t = obj.string_call("fs_get_terminal");
    end

    function s = file_size(obj, file)
      s = calllib(obj.lib, "fs_file_size", char(file));
    end
  end

  methods (Access = private)
    function lib = load_clib(~, libpath, incpath)
      arguments
        ~
        libpath (1,1) string
        incpath (1,1) string = fullfile(libpath, "../include")
      end

      lib = "ffilesystem";
      libpath = fullfile(libpath, lib);
      hdr = fullfile(incpath, "ffilesystem.h");

      if ~libisloaded(lib)
        [stat, warnings] = loadlibrary(libpath, hdr); %#ok<ASGLU>
      end
    end

    function t = string_call(obj, fun)
      s = string(pad("", obj.max_path));
      buf = libpointer('string', s);
      L = calllib(obj.lib, fun, buf, obj.max_path);
      if L > 0
        t = buf.value;
      else
        t = string.empty;
      end
    end

  end
end
