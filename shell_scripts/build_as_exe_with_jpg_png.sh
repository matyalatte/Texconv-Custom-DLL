#!/usr/bin/env bash
# Builds texconv and texassemble with static linked 3rd party libraries.
# It generates texconv and texassemble in the project root (Texconv-Custom-DLL/)

pushd $(dirname "$0") > /dev/null
./build.sh --build-as-exe --use-optional-formats "$@"
popd > /dev/null
