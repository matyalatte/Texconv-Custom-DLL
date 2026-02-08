@echo off

REM Builds texconv.dll with CMake and Visual Studio.
REM texconv.dll will be generated in ..\
REM The built dll requires vcruntime140.dll

%~dp0\build.bat --use-exr --runtime-dll %*
