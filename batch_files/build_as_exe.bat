@echo off

REM Builds executables with CMake and Visual Studio.
REM texconv.exe and texassemble will be generated in ..\

REM You need Visual Studio to use this batch file.

set VS_VERSION=Visual Studio 17 2022

mkdir %~dp0\..\build_exe
@pushd %~dp0\..\build_exe

cmake -G "%VS_VERSION%"^
 -D CMAKE_CONFIGURATION_TYPES=Release^
 -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded^
 -D TEXCONV_BUILD_AS_EXE=ON^
 -D TEXCONV_USE_ALL=ON^
 ../

cmake --build . --config Release
copy bin\Release\texconv.exe ..\
copy bin\Release\texassemble.exe ..\
@popd

pause
