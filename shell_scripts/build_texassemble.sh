#!/bin/bash
# Builds texconv with cmake.
# texconv will be generated in ./Texconv-Custom-DLL/

pushd $(dirname "$0")/../
mkdir build_exe
cd build_exe
cmake \
  -D CMAKE_BUILD_TYPE=Release\
  -D BUILD_DX11=OFF\
  -D BUILD_DX12=OFF\
  -D CMAKE_POSITION_INDEPENDENT_CODE=ON\
  -D TEXCONV_BUILD_AS_EXE=ON\
  -D TEXCONV_USE_USAGE=ON\
  -D TEXCONV_USE_TEXASSEMBLE=ON\
  ../
cmake --build .
cp bin/CMake/texassemble ../
popd