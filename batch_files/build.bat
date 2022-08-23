@echo off

REM Builds Texconv.dll with msbuild
REM Texconv.dll will be generated in Texconv-Custom-DLL\build\bin\CMake\Release

REM You need Visual Studio to use this batch file.
REM Run it with "x64 Ntive Tools Command Prompt for VS 2022". Or fail to build.

set VS_VERSION=Visual Studio 17 2022

mkdir ..\build
@pushd ..\build

cmake -G "%VS_VERSION%"^
 -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded^
 ../

msbuild Texconv.vcxproj /t:build /p:configuration=Release /p:platform=x64 -maxcpucount

@popd

copy ..\build\bin\CMake\Release\Texconv.dll ..\
