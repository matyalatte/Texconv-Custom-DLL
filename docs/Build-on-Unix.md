# Building Workflow for Linux and MacOS

You can build texconv on Linux and macOS.
However, please note that there are some limitations.  

-   Unable to use GPU for conversion.
-   Unable to use WIC supported formats (e.g. `.bmp`) except for `.jpg` and `.png`.

Also, the offical Texconv only supports Windows. I made sure I could build it with the following platforms and compilers but it might not work on your environment.  

-   Ubuntu 20.04 + GCC 9.4
-   MacOS 11 + AppleClang 13.0

## 0. Requirements

- c++17 compiler
- cmake
- git
- bash
- nasm (for jpeg support)

## 1. Get submodules

Move to `./Texconv-Custom-DLL` and run `git submodule update --init --recursive`.

```
git submodule update --init --recursive
```

It downloads DirectX related files on the repository.

## 2. Build the binary

You can build `libtexconv.so` (or `libtexconv.dylib`) with `./shell_scripts/build.sh`.

```
./shell_scripts/build.sh --use-optional-formats
```

It generates `libtexconv.so` (or `libtexconv.dylib`) in `./Texconv-Custom-DLL/`.  

## 3. Build executables (optional)

If you want executables, add `--build-as-exe` to options.

```
./shell_scripts/build.sh --use-optional-formats --build-as-exe
```

It generates `texconv` and `texassemble` in `./Texconv-Custom-DLL/`.  
You can use the built binaries on the terminal. (e.g. `./texconv -ft tga -y -o outdir -- test.dds`)  

## 4. More options

There are more options for `build.sh`.

```shell
> ./shell_scripts/build.sh --help
Usage: build.sh <options>
  --build-as-exe          build texconv and texassemble as executables
  --use-optional-formats  use 3rd party libraries to support non-DDS formats
  --use-dynamic-link      use dynamic linked 3rd party libraries
  --debug                 enable debug build
  --universal             build universal binary for macOS
  --no-texassemble        do not build texassemble

  Examples:
    build.sh
      -> generates libtexconv.so in the project root (Texconv-Custom-DLL/)
    build.sh --build-as-exe
      -> generates texconv and texassemble in the project root.
    build.sh --use-optional-formats
      -> generates libtexconv.so with JPEG and PNG support.
    build.sh --use-optional-formats --build-as-exe
      -> generates texconv and texassemble with JPEG and PNG support.
```
