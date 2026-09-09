---
description: when building the project code, use CMake presets
applyTo: '**/*.cmake, **/CMakeLists.txt, **/CMakePresets.json, **/*.cpp, **/*.h'
---

* build and test project code: `cmake --workflow default`
* only build the project code-this disables tests, so don't use this when working on tests: `cmake --workflow build`
* if working with project tests, use `cmake --build build` to build the project code and tests, then use `ctest --test-dir build` to run the tests using `-R` to specify which tests to run, e.g. `ctest --test-dir build -R copyfile` to run only the `copyfile` test

## Windows

On Windows when wanting to use WSL, the `cmake --workflow --preset linux` workflow uses /tmp/build for the build directory, to avoid conflicts with the native Windows build directory and filesystem.
When testing on a Windows computer, test with both a native Windows compiler as well as WSL to help avoid non-Windows breakage.
