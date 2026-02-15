#!/usr/bin/env bash
# Build and run tests


ESC=$(printf '\033')
echo "${ESC}[33mDeprication Warning:"
echo "  shell_scripts/$(basename "$0") will be removed from 2027 onward."
echo "  Please use \"build.cmd --use-optional-formats --test\" instead.${ESC}[m"

$(dirname "$0")/../scripts/build.sh --use-optional-formats --test "$@" || exit 1
