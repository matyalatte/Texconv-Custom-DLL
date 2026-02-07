@echo off

REM Builds executables with CMake and Visual Studio.
REM texconv.exe and texassemble will be generated in ..\

REM You need Visual Studio to use this batch file.

%~dp0\build.bat --build-as-exe %*
