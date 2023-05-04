@echo off

REM Builds texconv.dll without wic support and some minor options.
REM texconv.dll will be generated in ..\

set VS_VERSION=Visual Studio 17 2022

mkdir %~dp0\..\build
@pushd %~dp0\..\build

cmake -G "%VS_VERSION%"^
 -A x64^
 -D CMAKE_CONFIGURATION_TYPES=Release^
 -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded^
 -D TEXCONV_USE_LEGACY=ON^
 -D TEXCONV_USE_16BPP=ON^
 -D TEXCONV_USE_PLANAR=ON^
 -D TEXCONV_USE_MINOR_DDS_CONFIG=ON^
 -D TEXCONV_USE_3D=ON^
 -D TEXCONV_USE_TEXASSEMBLE=ON^
 -D TEXCONV_USE_FILTER=ON^
 -D BC_USE_OPENMP=OFF^
 ../

cmake --build . --config Release
copy bin\CMake\Release\texconv.dll ..\
@popd

