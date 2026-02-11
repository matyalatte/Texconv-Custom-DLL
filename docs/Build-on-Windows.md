# Building Workflow for Windows

## 0. Requirements

-   c++17 compiler
-   cmake
-   git
-   fxc (included in [Windows SDK](https://learn.microsoft.com/en-us/windows/apps/windows-sdk/downloads))

> [!note]
> It is recommended to use Visual Studio as a build system because it can install Windows SDK and find fxc without editing environment variables. Also, MinGW's fxc ([mingw-w64-fxc2](https://packages.msys2.org/base/mingw-w64-fxc2)) does not work in this project.

## 1. Get DirectXTex
Move to `.\Texconv-Custom-DLL` and run `git submodule update --init --recursive`.

```
git submodule update --init --recursive
```

## 2. Build DLL

You can build `texconv.dll` with `.\batch_files\build.bat`.

```
.\batch_files\build.bat --use-optional-formats
```

It generates `texconv.dll` in `Texconv-Custom-DLL\`.  

## 3. Build executables (optional)

If you want executables, add `--build-as-exe` to options.

```
.\batch_files\build.bat --use-optional-formats --build-as-exe
```

It generates `texconv.exe` and `texassemble.exe` in `.\Texconv-Custom-DLL\`.  
You can use the built binaries on the command prompt. (e.g. `.\texconv.exe -ft tga -y -o outdir -- test.dds`)  

## 4. More options

There are more options for `build.bat`.

```console
Usage: build.bat <options>
  --build-as-exe    build texconv and texassemble as executables
  --runtime-dll     use dynamic linked vcruntime
  --debug           enable debug build
  --test            build and run tests
  --no-wic          disable WIC supported formats (JPEG, PNG, etc.)
  --no-texassemble  do not build texassemble
  --use-exr         support EXR format
  --use-optional-formats  same as --use-exr
  --generator       one of CMake Generators e.g. Visual Studio 17 2022

  Examples:
    build.bat
      generates texconv.dll in the project root (Texconv-Custom-DLL/)
    build.bat --build-as-exe
      generates texconv.exe and texassemble.exe in the project root.
    build.bat --build-as-exe --generator "MinGW Makefiles"
      builds texconv.exe with MinGW's make
```
