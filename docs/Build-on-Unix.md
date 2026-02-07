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
It downloads DirectX related files on the repository.

## 2. Build the binary with a shell script

You can build `libtexconv.so` (or `libtexconv.dylib`) with a shell script.  
Run `./shell_scripts/build.sh` on the terminal.  
(Or run `./shell_scripts/build_with_jpg_png.sh` to support `.jpg` and `.png`.)  
It generates `libtexconv.so` (or `libtexconv.dylib`) in `./Texconv-Custom-DLL/`.  

## 3. Build executables (optional)

If you want an executable, use `build_as_exe.sh` instead of `build.sh`.  
(Or use `build_as_exe_with_jpg_png.sh` to support `.jpg` and `.png`.)  
It generates `texconv` and `texassemble` in `./Texconv-Custom-DLL/`.  
You can use them on the terminal. (e.g. `./texconv -ft tga -y -o outdir test.dds`)  
