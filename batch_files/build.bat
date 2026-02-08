@echo off
setlocal ENABLEDELAYEDEXPANSION

REM Builds texconv with cmake
REM and copies it to the project root (Texconv-Custom-DLL/)
REM The built binaries do NOT require vcruntime140.dll

set GENERATOR=Visual Studio 17 2022

set SCRIPT_DIR=%~dp0
set BUILD_DIR=build
set CONFIG_TYPE=Release
set BUILD_AS_EXE=OFF
set VCRUNTIME=MultiThreaded
set USE_WIC=ON
set USE_TEXASSEMBLE=ON

:parse_args
if "%~1"=="" goto end_parse

if "%~1"=="--debug" (
    set CONFIG_TYPE=Debug
) else if "%~1"=="--build-as-exe" (
    set BUILD_AS_EXE=ON
    set BUILD_DIR=build_exe
) else if "%~1"=="--generator" (
    if "%~2"=="" (
        echo Error: --generator requires another argument
        exit /b 1
    )
    set GENERATOR=%~2
    shift
) else if "%~1"=="--runtime-dll" (
    set VCRUNTIME=MultiThreadedDLL
) else if "%~1"=="--no-wic" (
    set USE_WIC=OFF
) else if "%~1"=="--no-texassemble" (
    set USE_TEXASSEMBLE=OFF
) else if "%~1"=="--help" (
    goto usage
) else (
    echo Error: unexpected argument: %~1
    goto usage
)
shift
goto parse_args
:end_parse
set CMAKE_OPTIONS=-G "%GENERATOR%"^
 -DCMAKE_CONFIGURATION_TYPES=%CONFIG_TYPE%^
 -DCMAKE_MSVC_RUNTIME_LIBRARY=%VCRUNTIME%^
 -DTEXCONV_BUILD_AS_EXE=%BUILD_AS_EXE%^
 -DTEXCONV_USE_WIC=%USE_WIC%^
 -DTEXCONV_USE_TEXASSEMBLE=%USE_TEXASSEMBLE%

mkdir %SCRIPT_DIR%\..\%BUILD_DIR%
@pushd %SCRIPT_DIR%\..\%BUILD_DIR%
dir
echo %CMAKE_OPTIONS%
cmake %CMAKE_OPTIONS% ../
if errorlevel 1 exit /b 1
cmake --build . --config %CONFIG_TYPE%
if errorlevel 1 exit /b 1
if "%BUILD_AS_EXE%"=="ON" (
    copy bin\%CONFIG_TYPE%\texconv.exe ..\
    if "%USE_TEXASSEMBLE%"=="ON" (
        copy bin\%CONFIG_TYPE%\texassemble.exe ..\
    )
) else (
    copy bin\%CONFIG_TYPE%\texconv.dll ..\
)
@popd

exit /b 0

:usage
echo Usage: build.bat ^<options^>
echo   --build-as-exe    build texconv and texassemble as executables
echo   --runtime-dll     use dynamic linked vcruntime
echo   --debug           enable debug build
echo   --no-wic          disable WIC supported formats (JPEG, PNG, etc.)
echo   --no-texassemble  do not build texassemble
echo.
echo   Examples:
echo     build.bat
echo       generates texconv.dll in the project root (Texconv-Custom-DLL/)
echo     build.bat --build-as-exe
echo       generates texconv.exe and texassemble.exe in the project root.
echo     build.bat --build-as-exe --no-texassemble --no-wic
echo       generates texconv.exe without WIC support.
exit /b 0
