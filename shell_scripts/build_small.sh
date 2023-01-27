#!/bin/bash
# Builds texconv without some cmake options.
# libtexconv.so or libtexconv.dylib will be generated in ./Texconv-Custom-DLL/

pushd $(dirname "$0")/../
mkdir build
cd build
cmake \
  -D CMAKE_BUILD_TYPE=Release\
  -D BUILD_DX11=OFF\
  -D BUILD_DX12=OFF\
  -D CMAKE_POSITION_INDEPENDENT_CODE=ON\
  -D TEXCONV_USE_LEGACY=ON\
  -D TEXCONV_USE_16BPP=ON\
  -D TEXCONV_USE_PLANAR=ON\
  -D TEXCONV_USE_MINOR_DDS_CONFIG=ON\
  -D TEXCONV_USE_3D=ON\
  -D TEXCONV_USE_TEXASSEMBLE=ON\
  -D TEXCONV_USE_FILTER=ON\
  ../
cmake --build .
cp bin/CMake/libtexconv.* ../
popd