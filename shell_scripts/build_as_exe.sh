#!/usr/bin/env bash
# Builds texconv and texassemble without 3rd party libraries.
# It generates texconv and texassemble in the project root (Texconv-Custom-DLL/)

ESC=$(printf '\033')
echo "${ESC}[33mDeprecation Warning:"
echo "  shell_scripts/$(basename "$0") will be removed from 2027 onward."
echo "  Please use \"build.cmd --build-as-exe\" instead.${ESC}[m"

$(dirname "$0")/../scripts/build.sh --build-as-exe "$@" || exit 1
