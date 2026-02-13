#!/usr/bin/env bash
# Builds texconv with cmake
# and copy it to the project root (Texconv-Custom-DLL/)

function usage() {
cat <<EOF
Usage: build.cmd [options]
  --build-as-exe          build texconv and texassemble as executables
  --use-optional-formats  support JPEG, PNG, and EXR formats
  --use-dynamic-link      use system installed 3rd party libraries
  --no-texassemble        do not build texassemble
  --debug                 enable debug build
  --test                  build and run tests
  --generator <name>      specify a build system e.g. Ninja
  --universal             build universal binary for macOS

  Examples:
    build.cmd
      -> generates libtexconv in the project root (Texconv-Custom-DLL/)
    build.cmd --build-as-exe
      -> generates texconv and texassemble in the project root.
    build.cmd --use-optional-formats
      -> generates libtexconv with JPEG, PNG, and EXR support.
    build.cmd --build-as-exe --generator "Unix Makefiles"
      -> generates texconv and texassemble with make.
EOF
}

os="$(uname -s)"
cmake_options=(
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON
)
config_type=Release
build_universal=false
use_dynamic_link=false
use_optional_formats=false
build_as_exe=false
use_texassemble=true
build_dir=build
test=false
generator="$(
  cmake --help \
  | sed -nE 's/^\* (.*[^[:space:]])[[:space:]]*=.*/\1/p'
)"
next_arg_is_generator=false

for arg in "$@"; do
  if $next_arg_is_generator; then
    generator="$arg"
    next_arg_is_generator=false
    continue
  fi
  case "$arg" in
    --debug)
      config_type=Debug
      ;;
    --test)
      test=true
      ;;
    --universal)
      if [[ "$os" == "Darwin" ]]; then
        build_universal=true
      else
        echo "Warning: --universal is only available on macOS"
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
    --generator)
      next_arg_is_generator=true
      ;;
    --runtime-dll|--no-wic|--use-exr)
      echo Warning: $arg is only available on Windows.
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
cmake_options+=("-DCMAKE_BUILD_TYPE=${config_type}")
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

echo "cmake -G \"${generator}\" ${cmake_options[@]} ../"
cmake_options=(
  -G "${generator}"
  "${cmake_options[@]}"
)

bin_dir=bin
lib_dir=lib
if [[ "$generator" == "Xcode" ||
      "$generator" == "Ninja Multi-Config" ]]; then
  bin_dir="bin/${config_type}"
  lib_dir="lib/${config_type}"
fi

strip_symbols() {
  local obj=$1
  if [[ "$config_type" == "Release" ]]; then
    if [[ "$os" == "Linux" ]]; then
      strip --strip-all $obj
    elif [[ "$os" == "Darwin" ]]; then
      strip $obj
    fi
  fi
}

strip_texconv() {
  if $build_as_exe; then
    strip_symbols "${bin_dir}/texconv"
    if $use_texassemble; then
      strip_symbols "${bin_dir}/texassemble"
    fi
  else
    strip_symbols "${lib_dir}/libtexconv.*"
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
      lipo -create -output texconv "${build_dir}_x86_64/${bin_dir}/texconv" "${build_dir}_arm64/${bin_dir}/texconv"
      if $use_texassemble; then
        lipo -create -output texassemble "${build_dir}_x86_64/${bin_dir}/texassemble" "${build_dir}_arm64/${bin_dir}/texassemble"
      fi
    else
      lipo -create -output libtexconv.dylib "${build_dir}_x86_64/${lib_dir}/libtexconv.dylib" "${build_dir}_arm64/${lib_dir}/libtexconv.dylib"
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
      cp "${bin_dir}/texconv" ../
      if $use_texassemble; then
        cp "${bin_dir}/texassemble" ../
      fi
    else
      cp "${lib_dir}/libtexconv."* ../
    fi
  fi
popd > /dev/null
