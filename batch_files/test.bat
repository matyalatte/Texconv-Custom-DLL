@echo off

REM Build and run tests

echo %ESC%[33mDeprecation Warning:
echo   shell_scripts/test.bat will be removed from 2027 onward.
echo   Please use 'build.cmd --test' instead.%ESC%[0m

%~dp0\build.bat --test %*
