REM batch file since Github Actions doesn't support shell cmd well,
REM and cmd is needed for oneAPI Windows

call "C:\Program Files (x86)\Intel\oneAPI\setvars.bat"
if %errorlevel% neq 0 exit /b %errorlevel%

call "C:\Program Files (x86)\Intel\oneAPI\compiler\latest\env\vars.bat
if %errorlevel% neq 0 exit /b %errorlevel%

echo "configure %GITHUB_REPOSITORY%"
cmake --preset default --install-prefix %RUNNER_TEMP%
if %errorlevel% neq 0 (
  type build\CMakeFiles\CMakeConfigureLog.yaml & exit /b %errorlevel%
)

echo "workflow %GITHUB_REPOSITORY%"
cmake --workflow default
if %errorlevel% neq 0 exit /b %errorlevel%

echo "install project"
cmake --install build
if %errorlevel% neq 0 exit /b %errorlevel%

echo "Example config, build, test"
cd example
set CMAKE_PREFIX_PATH=%RUNNER_TEMP%
cmake --workflow default
if %errorlevel% neq 0 exit /b %errorlevel%
