# Building Workflow for Windows

## 0. Requirements

-   Visual Studio 2022
-   CMake
-   git

## 1. Get DirectXTex
Move to `.\Texconv-Custom-DLL` and run `git submodule update --init --recursive DirectXTex`.

```
git submodule update --init --recursive DirectXTex
```

It downloads DirectXTex to the repository.  
(`git submodule update --init --recursive` works as well, but it downloads unnecessary repositories for Windows.)  

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

There are more options for `build.sh`.

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

  Examples:
    build.bat
      generates texconv.dll in the project root (Texconv-Custom-DLL/)
    build.bat --build-as-exe
      generates texconv.exe and texassemble.exe in the project root.
    build.bat --build-as-exe --no-texassemble --no-wic
      generates texconv.exe without WIC support.
```
