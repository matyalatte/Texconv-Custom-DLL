#!/usr/bin/env bash
# Build and run tests

pushd $(dirname "$0") > /dev/null
./build.sh --use-optional-formats --test "$@"
popd > /dev/null
