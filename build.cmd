:<<'END'
@echo off
rem build.cmd works as a batch file on Windows.
%~dp0\batch_files\build.bat %*
goto :eof
END

# build.cmd works as a shell script on Linux/macOS.
#!/usr/bin/env bash
$(dirname "$0")/shell_scripts/build.sh "$@" || exit 1
