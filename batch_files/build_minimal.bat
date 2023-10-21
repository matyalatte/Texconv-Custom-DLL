@echo off

REM Make a minimal build for texconv.dll.
REM texconv.dll will be generated in ..\

set VS_VERSION=Visual Studio 17 2022

mkdir %~dp0\..\build
@pushd %~dp0\..\build

cmake -G "%VS_VERSION%"^
 -A x64^
 -D CMAKE_CONFIGURATION_TYPES=Release^
 -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded^
 -D BC_USE_OPENMP=OFF^
 ../

cmake --build . --config Release
copy bin\CMake\Release\texconv.dll ..\
@popd

