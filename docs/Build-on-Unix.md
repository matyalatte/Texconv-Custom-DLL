# Building Workflow for Linux and MacOS

You can build texconv on Linux and macOS.
However, please note that there are some limitations.  

-   Unable to use GPU for conversion.
-   Unable to use WIC supported formats (e.g. `.bmp`) except for `.jpg` and `.png`.

## 0. Requirements

- c++17 compiler
- cmake
- git
- bash
- nasm (for jpeg support)

## 1. Get submodules

Move to `./Texconv-Custom-DLL` and get submodules.

```
git submodule update --init --recursive
```

## 2. Build the binary

You can build `libtexconv.so` (or `libtexconv.dylib`) with `./build.cmd`.

```
./build.cmd --use-optional-formats
```

It generates `libtexconv.so` (or `libtexconv.dylib`) in `./Texconv-Custom-DLL/`.  

## 3. Build executables (optional)

If you want executables, add `--build-as-exe` to options.

```
./build.cmd --use-optional-formats --build-as-exe
```

It generates `texconv` and `texassemble` in `./Texconv-Custom-DLL/`.  
You can use the built binaries on the terminal. (e.g. `./texconv -ft tga -y -o outdir -- test.dds`)  

## 4. More options

There are more options for `build.cmd`.

```console
> ./build.cmd --help
Usage: build.cmd [options]
  --build-as-exe          build texconv and texassemble as executables
  --use-optional-formats  support JPEG, PNG, and EXR formats
  --use-dynamic-link      use system installed 3rd party libraries
  --no-texassemble        do not build texassemble
  --debug                 enable debug build
  --test                  build and run tests
  --generator <name>      specify a build system e.g. Ninja
  --universal             build universal binary for macOS

  Examples:
    build.cmd
      -> generates libtexconv in the project root (Texconv-Custom-DLL/)
    build.cmd --build-as-exe
      -> generates texconv and texassemble in the project root.
    build.cmd --use-optional-formats
      -> generates libtexconv with JPEG, PNG, and EXR support.
    build.cmd --build-as-exe --generator "Unix Makefiles"
      -> generates texconv and texassemble with make.
```
