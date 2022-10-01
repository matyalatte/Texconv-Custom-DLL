@echo off

REM Builds texconv.dll with msbuild
REM texconv.dll will be generated in ..\

REM You need Visual Studio to use this batch file.

set VS_VERSION=Visual Studio 17 2022

mkdir ..\build
@pushd ..\build

cmake -G "%VS_VERSION%"^
 -A x64^
 -D CMAKE_CONFIGURATION_TYPES=Release^
 -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded^
 -D TEXCONV_USE_LEGACY=ON^
 -D TEXCONV_USE_16BPP=ON^
 -D TEXCONV_USE_PLANAR=ON^
 -D TEXCONV_USE_MINOR_DDS_CONFIG=ON^
 ../

cmake --build . --config Release


@popd

copy ..\build\bin\CMake\Release\texconv.dll ..\