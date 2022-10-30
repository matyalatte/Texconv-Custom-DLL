@echo off

REM Builds texconv.dll with CMake and Visual Stdio.
REM texconv.dll will be generated in ..\

set VS_VERSION=Visual Studio 17 2022

mkdir %~dp0\..\build
@pushd %~dp0\..\build

cmake -G "%VS_VERSION%"^
 -A x64^
 -D CMAKE_CONFIGURATION_TYPES=Release^
 -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded^
 ../

cmake --build . --config Release
copy bin\CMake\Release\texconv.dll ..\
@popd

