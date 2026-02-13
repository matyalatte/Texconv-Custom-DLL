@echo off
setlocal ENABLEDELAYEDEXPANSION

REM Builds texconv with cmake
REM and copies it to the project root (Texconv-Custom-DLL/)
REM The built binaries do NOT require vcruntime140.dll

REM get default generator
for /f "tokens=1,* delims=*=" %%A in ('
    cmake --help ^| findstr /B "*"
') do (
    set "GEN=%%A"
    :trim
    if "!GEN:~0,1!"==" " (
        set "GEN=!GEN:~1!"
        goto trim
    )
    :trim_end
    if "!GEN:~-1!"==" " (
        set "GEN=!GEN:~0,-1!"
        goto trim_end
    )
)
set GENERATOR=!GEN!
if "%GENERATOR%" == "" (
    set GENERATOR=Visual Studio 17 2022
)

set SCRIPT_DIR=%~dp0
set BUILD_DIR=build
set CONFIG_TYPE=Release
set BUILD_AS_EXE=OFF
set VCRUNTIME=MultiThreaded
set USE_WIC=ON
set USE_EXR=OFF
set USE_STATIC=ON
set USE_TEXASSEMBLE=ON
set TEST=OFF

:parse_args
if "%~1"=="" goto end_parse

if "%~1"=="--debug" (
    set CONFIG_TYPE=Debug
) else if "%~1"=="--test" (
    set TEST=ON
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
) else if "%~1"=="--use-exr" (
    set USE_EXR=ON
) else if "%~1"=="--use-optional-formats" (
    set USE_EXR=ON
) else if "%~1"=="--use-dynamic-link" (
    set USE_STATIC=OFF
) else if "%~1"=="--universal" (
    echo Warning: --universal is only available on macOS
) else if "%~1"=="--help" (
    goto usage
) else (
    echo Error: unexpected argument: %~1
    goto usage
)
shift
goto parse_args
:end_parse

set BIN_DIR=bin
if "%GENERATOR:~0,13%"=="Visual Studio" (
    set BIN_DIR=bin\%CONFIG_TYPE%
) else if "%GENERATOR%"=="Ninja Multi-Config" (
    set BIN_DIR=bin\%CONFIG_TYPE%
)

set CMAKE_OPTIONS=-G "%GENERATOR%"^
 -DCMAKE_CONFIGURATION_TYPES=%CONFIG_TYPE%^
 -DCMAKE_BUILD_TYPE=%CONFIG_TYPE%^
 -DCMAKE_MSVC_RUNTIME_LIBRARY=%VCRUNTIME%^
 -DTEXCONV_BUILD_AS_EXE=%BUILD_AS_EXE%^
 -DTEXCONV_BUILD_TESTS=%TEST%^
 -DTEXCONV_USE_WIC=%USE_WIC%^
 -DTEXCONV_USE_TEXASSEMBLE=%USE_TEXASSEMBLE%^
 -DTEXCONV_USE_STATIC_LINK=%USE_STATIC%^
 -DENABLE_OPENEXR_SUPPORT=%USE_EXR%

mkdir %SCRIPT_DIR%\..\%BUILD_DIR%
@pushd %SCRIPT_DIR%\..\%BUILD_DIR%
echo cmake %CMAKE_OPTIONS% ../
cmake %CMAKE_OPTIONS% ../
if errorlevel 1 exit /b 1
cmake --build . --config %CONFIG_TYPE%
if errorlevel 1 exit /b 1
if "%TEST%"=="ON" (
    ctest --verbose -C %CONFIG_TYPE%
    if errorlevel 1 exit /b 1
)
if "%BUILD_AS_EXE%"=="ON" (
    copy %BIN_DIR%\texconv.exe ..\
    if "%USE_TEXASSEMBLE%"=="ON" (
        copy %BIN_DIR%\texassemble.exe ..\
    )
) else (
    copy %BIN_DIR%\texconv.dll ..\
)
@popd

exit /b 0

:usage
echo Usage: build.cmd ^[options^]
echo   --build-as-exe          build texconv and texassemble as executables
echo   --use-optional-formats  support EXR format
echo   --use-dynamic-link      use system installed OpenEXR
echo   --no-texassemble        do not build texassemble
echo   --debug                 enable debug build
echo   --test                  build and run tests
echo   --generator ^<name^>      specify a build system e.g. Ninja
echo   --runtime-dll           use dynamic linked vcruntime
echo   --no-wic                disable WIC supported formats (JPEG, PNG, etc.)
echo   --use-exr               same as --use-optional-formats
echo.
echo   Examples:
echo     build.cmd
echo       generates texconv.dll in the project root (Texconv-Custom-DLL/)
echo     build.cmd --build-as-exe
echo       generates texconv.exe and texassemble.exe in the project root.
echo     build.cmd --use-optional-formats
echo       generates texconv.dll with EXR support
echo     build.cmd --build-as-exe --generator "MinGW Makefiles"
echo       builds texconv.exe with MinGW's make
exit /b 0
