#!/bin/bash
# Builds executables with cmake.
# texconv and texassemble will be generated in ./Texconv-Custom-DLL/

pushd $(dirname "$0")/../
mkdir build_exe
cd build_exe
cmake \
  -D CMAKE_BUILD_TYPE=Release\
  -D BUILD_DX11=OFF\
  -D BUILD_DX12=OFF\
  -D CMAKE_POSITION_INDEPENDENT_CODE=ON\
  -D TEXCONV_BUILD_AS_EXE=ON\
  -D TEXCONV_USE_ALL=ON\
  -D ENABLE_LIBJPEG_SUPPORT=ON\
  -D ENABLE_LIBPNG_SUPPORT=ON\
  ../
cmake --build .
cp bin/texconv ../
cp bin/texassemble ../
popd
