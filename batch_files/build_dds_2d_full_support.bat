@echo off

REM Builds texconv.dll with msbuild
REM texconv.dll will be generated in ..\

REM You need Visual Studio to use this batch file.
REM Run it with "x64 Ntive Tools Command Prompt for VS 2022". Or fail to build.

set VS_VERSION=Visual Studio 17 2022

mkdir ..\build
@pushd ..\build

cmake -G "%VS_VERSION%"^
 -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded^
 -D TEXCONV_USE_LEGACY=ON^
 -D TEXCONV_USE_16BPP=ON^
 -D TEXCONV_USE_PLANAR=ON^
 -D TEXCONV_USE_MINOR_DDS_CONFIG=ON^
 ../

msbuild texconv.vcxproj /t:build /p:configuration=Release /p:platform=x64 -maxcpucount

@popd

copy ..\build\bin\CMake\Release\texconv.dll ..\
