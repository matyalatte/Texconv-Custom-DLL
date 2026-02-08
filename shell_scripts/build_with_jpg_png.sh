#!/usr/bin/env bash
# Builds libtexconv.so with static linked 3rd party libraries.
# It generates libtexconv.so or libtexconv.dylib in the project root (Texconv-Custom-DLL/)

pushd $(dirname "$0") > /dev/null
./build.sh --use-optional-formats "$@" || exit 1
popd > /dev/null
