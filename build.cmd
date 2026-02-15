:<<'END'
@echo off
rem build.cmd works as a batch file on Windows.
%~dp0\scripts\build.bat %*
goto :eof
END

# build.cmd works as a shell script on Linux/macOS.
#!/usr/bin/env bash
$(dirname "$0")/scripts/build.sh "$@" || exit 1
