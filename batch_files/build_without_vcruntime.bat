@echo off

REM Builds texconv.dll with CMake and Visual Studio.
REM texconv.dll will be generated in ..\
REM The built dll requires vcruntime140.dll

echo %ESC%[33mDeprecation Warning:
echo   batch_files/build_without_vcruntime.bat will be removed from 2027 onward.
echo   Please use 'build.cmd --use-exr --runtime-dll' instead.%ESC%[0m

%~dp0\build.bat --use-exr --runtime-dll %*
