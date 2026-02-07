#!/usr/bin/env bash
# Builds universal binary with cmake.
# libtexconv.dylib will be generated in ./Texconv-Custom-DLL/

cmake_options=(
  -DCMAKE_BUILD_TYPE=Release
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON
  -DTEXCONV_USE_ALL=ON
)
use_optional_formats=false
build_as_exe=false
build_dir=build

for arg in "$@"; do
  case "$arg" in
    --use-optional-formats)
      use_optional_formats=true
      cmake_options+=(
        -DTEXCONV_USE_STATIC_LINK=ON
        -DENABLE_LIBJPEG_SUPPORT=ON
        -DENABLE_LIBPNG_SUPPORT=ON
      )
      ;;
    --build-as-exe)
      build_as_exe=true
      cmake_options+=(
        -DTEXCONV_BUILD_AS_EXE=ON
      )
      build_dir+="_exe"
      ;;
  esac
done

build_arch() {
  local arch=$1
  mkdir -p "${build_dir}_${arch}"
  cd "${build_dir}_${arch}"
  cmake "${cmake_options[@]}" \
    -DCMAKE_SYSTEM_PROCESSOR="$arch" \
    -DCMAKE_OSX_ARCHITECTURES="$arch" \
    ..
  cmake --build .
  cd ..
}


pushd $(dirname "$0")/../
if $use_optional_formats; then
  build_arch x86_64
  build_arch arm64

  if $build_as_exe; then
    lipo -create -output texconv "${build_dir}_x86_64/bin/texconv" "${build_dir}_arm64/bin/texconv"
    lipo -create -output texassemble "${build_dir}_x86_64/bin/texassemble" "${build_dir}_arm64/bin/texassemble"
  else
    lipo -create -output libtexconv.dylib "${build_dir}_x86_64/lib/libtexconv.dylib" "${build_dir}_arm64/lib/libtexconv.dylib"
  fi
else
  mkdir -p build
  cd build
  cmake "${cmake_options[@]}" \
    -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"\
    ../
  cmake --build .
  if $build_as_exe; then
    cp bin/texconv ../
    cp bin/texassemble ../
  else
    cp lib/libtexconv.* ../
  fi
fi
popd
