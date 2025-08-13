@echo off

REM Builds texconv.dll with CMake and Visual Studio.
REM texconv.dll will be generated in ..\
REM The built dll requires vcruntime140.dll

set VS_VERSION=Visual Studio 17 2022

mkdir %~dp0\..\build
@pushd %~dp0\..\build

cmake -G "%VS_VERSION%"^
 -D CMAKE_CONFIGURATION_TYPES=Release^
 -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL^
 -D TEXCONV_USE_ALL=ON^
 ../

cmake --build . --config Release
copy bin\Release\texconv.dll ..\
@popd

pause
