#!/usr/bin/env bash
# Builds universal binary with cmake.
# libtexconv.dylib will be generated in ./Texconv-Custom-DLL/

ESC=$(printf '\033')
echo "${ESC}[33mDeprecation Warning:"
echo "  shell_scripts/$(basename "$0") will be removed from 2027 onward."
echo "  Please use build.cmd instead.${ESC}[m"

$(dirname "$0")/../scripts/build.sh "$@" || exit 1
