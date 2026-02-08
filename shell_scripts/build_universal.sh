#!/usr/bin/env bash
# Builds universal binary with cmake.
# libtexconv.dylib will be generated in ./Texconv-Custom-DLL/

pushd $(dirname "$0") > /dev/null
./build.sh --universal "$@" || exit 1
popd > /dev/null
