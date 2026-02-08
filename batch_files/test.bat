@echo off

REM Build and run tests

%~dp0\build.bat --use-exr --test %*
