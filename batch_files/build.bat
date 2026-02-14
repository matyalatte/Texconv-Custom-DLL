@echo off

REM Builds executables with CMake
REM texconv.exe and texassemble will be generated in ..\

for /f %%i in ('cmd /k prompt $e^<nul') do set ESC=%%i

echo %ESC%[33mDeprecation Warning:
echo   batch_files/build.bat will be removed from 2027 onward.
echo   Please use build.cmd instead.%ESC%[0m

%~dp0\..\scripts\build.bat %*
