# Building Workflow for Windows

## 0. Requirements

-   c++17 compiler
-   cmake
-   git
-   fxc (included in [Windows SDK](https://learn.microsoft.com/en-us/windows/apps/windows-sdk/downloads))

> [!note]
> It is recommended to use Visual Studio as a build system because it can install Windows SDK and find fxc without editing environment variables. Also, MinGW's fxc ([mingw-w64-fxc2](https://packages.msys2.org/base/mingw-w64-fxc2)) does not work in this project.

## 1. Get submodules

Move to `.\Texconv-Custom-DLL` and get submodules.

```
git submodule update --init --recursive
```

## 2. Build DLL

You can build `texconv.dll` with `.\build.cmd`.

```
.\build.cmd --use-optional-formats
```

It generates `texconv.dll` in `Texconv-Custom-DLL\`.  

## 3. Build executables (optional)

If you want executables, add `--build-as-exe` to options.

```
.\build.cmd --use-optional-formats --build-as-exe
```

It generates `texconv.exe` and `texassemble.exe` in `.\Texconv-Custom-DLL\`.  
You can use the built binaries on the command prompt. (e.g. `.\texconv.exe -ft tga -y -o outdir -- test.dds`)  

## 4. More options

There are more options for `build.cmd`.

```console
Usage: build.cmd [options]
  --build-as-exe          build texconv and texassemble as executables
  --use-optional-formats  support EXR format
  --use-dynamic-link      use system installed OpenEXR
  --no-texassemble        do not build texassemble
  --debug                 enable debug build
  --test                  build and run tests
  --generator <name>      specify a build system e.g. Ninja
  --runtime-dll           use dynamic linked vcruntime
  --no-wic                disable WIC supported formats (JPEG, PNG, etc.)
  --use-exr               same as --use-optional-formats

  Examples:
    build.cmd
      generates texconv.dll in the project root (Texconv-Custom-DLL/)
    build.cmd --build-as-exe
      generates texconv.exe and texassemble.exe in the project root.
    build.cmd --use-optional-formats
      generates texconv.dll with EXR support
    build.cmd --build-as-exe --generator "MinGW Makefiles"
      builds texconv.exe with MinGW's make
```
