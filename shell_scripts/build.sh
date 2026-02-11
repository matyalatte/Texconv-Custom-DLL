#!/usr/bin/env bash
# Builds texconv with cmake
# and copy it to the project root (Texconv-Custom-DLL/)

function usage() {
cat <<EOF
Usage: build.sh <options>
  --build-as-exe          build texconv and texassemble as executables
  --use-optional-formats  use 3rd party libraries to support non-DDS formats
  --use-dynamic-link      use dynamic linked 3rd party libraries
  --debug                 enable debug build
  --test                  build and run tests
  --universal             build universal binary for macOS
  --no-texassemble        do not build texassemble

  Examples:
    build.sh
      -> generates libtexconv.so in the project root (Texconv-Custom-DLL/)
    build.sh --build-as-exe
      -> generates texconv and texassemble in the project root.
    build.sh --use-optional-formats
      -> generates libtexconv.so with JPEG, PNG, and EXR support.
    build.sh --use-optional-formats --build-as-exe
      -> generates texconv and texassemble with JPEG, PNG, EXR support.
EOF
}

os="$(uname -s)"
cmake_options=(
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON
)
debug=false
build_universal=false
use_dynamic_link=false
use_optional_formats=false
build_as_exe=false
use_texassemble=true
build_dir=build
test=false

for arg in "$@"; do
  case "$arg" in
    --debug)
      debug=true
      ;;
    --test)
      test=true
      ;;
    --universal)
      if [[ "$os" == "Darwin" ]]; then
        build_universal=true
      fi
      ;;
    --use-dynamic-link)
      use_dynamic_link=true
      ;;
    --use-optional-formats)
      use_optional_formats=true
      cmake_options+=(
        -DENABLE_LIBJPEG_SUPPORT=ON
        -DENABLE_LIBPNG_SUPPORT=ON
        -DENABLE_OPENEXR_SUPPORT=ON
      )
      ;;
    --build-as-exe)
      build_as_exe=true
      cmake_options+=(-DTEXCONV_BUILD_AS_EXE=ON)
      build_dir=build_exe
      ;;
    --no-texassemble)
      use_texassemble=false
      ;;
    --help)
      usage
      exit 0
      ;;
    *)
      echo "Error: unexpected argument: ${arg}"
      usage
      exit 1
      ;;
  esac
done
if $debug; then
  cmake_options+=(-DCMAKE_BUILD_TYPE=Debug)
else
  cmake_options+=(-DCMAKE_BUILD_TYPE=Release)
fi
if $test; then
  if $build_universal; then
    echo "Error: You can NOT build tests for universal binaries."
    exit 1
  fi
  cmake_options+=(-DTEXCONV_BUILD_TESTS=ON)
fi
if $use_dynamic_link; then
  cmake_options+=(-DTEXCONV_USE_STATIC_LINK=OFF)
else
  if $use_optional_formats; then
    if ! command -v nasm >/dev/null 2>&1; then
      echo "Error: libjpeg requires nasm to build SIMD extension."
      exit 1
    fi
  fi
  cmake_options+=(-DTEXCONV_USE_STATIC_LINK=ON)
fi
if $use_texassemble; then
  cmake_options+=(-DTEXCONV_USE_TEXASSEMBLE=ON)
fi

strip_symbols() {
  local obj=$1
  if [[ "$debug" == "false" ]]; then
    if [[ "$os" == "Linux" ]]; then
      strip --strip-all $obj
    elif [[ "$os" == "Darwin" ]]; then
      strip $obj
    fi
  fi
}

strip_texconv() {
  if $build_as_exe; then
    strip_symbols bin/texconv
    if $use_texassemble; then
      strip_symbols bin/texassemble
    fi
  else
    strip_symbols lib/libtexconv.*
  fi
}

cross_build() {
  # cross compile for macOS
  local arch=$1
  mkdir -p "${build_dir}_${arch}"
  cd "${build_dir}_${arch}"
  cmake "${cmake_options[@]}" \
    -DCMAKE_SYSTEM_PROCESSOR="$arch" \
    -DCMAKE_OSX_ARCHITECTURES="$arch" \
    .. || exit 1
  cmake --build . || exit 1
  strip_texconv
  cd ..
}

pushd $(dirname "$0")/../ > /dev/null
  if $build_universal; then
    cross_build x86_64
    cross_build arm64

    if $build_as_exe; then
      lipo -create -output texconv "${build_dir}_x86_64/bin/texconv" "${build_dir}_arm64/bin/texconv"
      if $use_texassemble; then
        lipo -create -output texassemble "${build_dir}_x86_64/bin/texassemble" "${build_dir}_arm64/bin/texassemble"
      fi
    else
      lipo -create -output libtexconv.dylib "${build_dir}_x86_64/lib/libtexconv.dylib" "${build_dir}_arm64/lib/libtexconv.dylib"
    fi
  else
    # standard build
    mkdir -p "${build_dir}"
    cd "${build_dir}"
    cmake "${cmake_options[@]}" ../ || exit 1
    cmake --build . || exit 1
    if $test; then
      ctest --verbose || exit 1
    fi
    strip_texconv

    # copy binaries
    if $build_as_exe; then
      cp bin/texconv ../
      if $use_texassemble; then
        cp bin/texassemble ../
      fi
    else
      cp lib/libtexconv.* ../
    fi
  fi
popd > /dev/null
