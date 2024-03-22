# Building Workflow for Ubuntu and MacOS

You can build texconv on Linux and macOS.  
But please note that there are some limitations.  

-   Unable to use GPU for conversion.
-   Unable to use WIC supported formats (`.bmp`, `.jpg`, `.png`, etc.)
    (Or requires libjpeg and libpng to support `.jpg` and `.png`.)

Also, the offical Texconv only supports Windows.  
I made sure I can build it with the following platforms and compilers.  
But it might not work on your environment.  

-   Ubuntu 20.04 + GCC 9.4
-   MacOS 10.15 + AppleClang 12.0
-   MacOS 11 + AppleClang 13.0

## 0. Requirements

- xcode (for macOS)
- build-essential (for Linux)
- cmake
- wget

## 1. Get submodules

Move to `./Texconv-Custom-DLL`.  
Then, type `git submodule update --init --recursive`.  
It'll install DirectX related files on the repository.

## 2. Get sal.h

On non-Windows platform, DirectXMath requires [sal.h](https://github.com/dotnet/corert/blob/master/src/Native/inc/unix/sal.h).  
Move to `./Texconv-Custom-DLL/shell_scripts` and type `bash get_sal.sh`.  
It will put the file into `./Texconv-Custom-DLL/unix_external/sal/`.  

## 3. Build the binary with a shell script

You can build `libtexconv.so` (or `libtexconv.dylib`) with a shell script.  
Run `bash shell_scripts/build.sh` on the terminal.  
(Or run `bash shell_scripts/build_with_jpg_png.sh` to support `.jpg` and `.png`.)  
`libtexconv.so` (or `libtexconv.dylib`) will be generated in `./Texconv-Custom-DLL/`.  

## 4. Build executables (optional)

If you want an executable, use `build_as_exe.sh` instead of `build.sh`.  
(Or use `build_as_exe_with_jpg_png.sh` to support `.jpg` and `.png`.)  
`texconv` and `texassemble` will be generated in `./Texconv-Custom-DLL/`.  
You can use them on the terminal. (e.g. `./texconv -ft tga -y -o outdir test.dds`)  
