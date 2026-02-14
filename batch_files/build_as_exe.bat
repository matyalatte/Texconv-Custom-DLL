@echo off

REM Builds executables with CMake.
REM texconv.exe and texassemble will be generated in ..\

echo %ESC%[33mDeprecation Warning:
echo   batch_files/build_as_exe.bat will be removed from 2027 onward.
echo   Please use 'build.cmd --use-exr --build-as-exe' instead.%ESC%[0m

%~dp0\build.bat --use-exr --build-as-exe %*
