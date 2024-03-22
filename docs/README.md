# Texconv-Custom-DLL v0.4.0

Cross-platform implementation for [Texconv](https://github.com/microsoft/DirectXTex/wiki/Texconv) and [Texassemble](https://github.com/Microsoft/DirectXTex/wiki/Texassemble).  
And you can use it as a DLL (or a shared library).  

## What's Texconv?

[Texconv](https://github.com/microsoft/DirectXTex/wiki/Texconv)
is a DDS converter developed by Microsoft.  
It supports all dds formats.  
There are some dds formats common tools can't handle properly, but Texconv can.  
It's the best DDS converter as far as I know.  

## Extra features

You can add some features by [cmake options](./CMake-Options.md).
-   Disable GPU codec (`TEXCONV_NO_GPU_CODEC`)
-   Enable to use [texassemble](https://github.com/microsoft/DirectXTex/wiki/Texassemble) as a DLL function (`TEXCONV_USE_TEXASSEMBLE`)
-   Enable to build [texassemble](https://github.com/microsoft/DirectXTex/wiki/Texassemble) as an executable (`TEXCONV_USE_TEXASSEMBLE` and `TEXCONV_BUILD_AS_EXE`)

## Platform

The official texconv only supports Windows.  
But the custom build supports the following platforms and compilers.

-   Windows 10 (or later) + MSVC
-   MacOS 10.15 (or later) + AppleClang
-   Linux + GCC 9.1 (or later)

## Download

You can download the built binary from [the release page](https://github.com/matyalatte/Texconv-Custom-DLL/releases).  

-   `TexconvCustomDLL*-Windows.zip` is for Windows.
-   `TexconvCustomDLL*-macOS.tar.bz2` is for macOS (10.15 or later).
-   `TexconvCustomDLL*-Linux.tar.bz2` is for Linux with GLIBC 2.27+ and GLIBCXX 3.4.26+.

> Each zip file has a DLL and executables.  
> You can copy whichever you want to use to your project.  
> (The executables don't refer the DLL.)

> Linux and macOS builds requrie libjpeg and libpng.  
> Or use `*-no-deps.tar.bz2` versions that don't support jpg and png.

> The linux build only supports distributions using GLIBC.  
> Build the executable by yourself if you want to use it on unsupported distros.

## Usage

See here if you want to know how to use texconv.  
[Usage](./Usage.md)  

## How to Build

### Windows

There is a document for Windows users.  
[Building Workflow for Windows](./Build-on-Windows.md)  

It only supports Visual Studio 2022, but you can see [the batch files](../batch_files/) to find a way to build it with your environment.  

### Ubuntu and Mac

There is a document for Mac and Ubuntu users.  
[Building Workflow for Ubuntu and MacOS](./Build-on-Unix.md)  
And you can see [the shell scripts](../shell_scripts/) to understand the workflow.  

### Docker

Linux users can use docker files to build the binary.  
See the files for the details.  

- [Dockerfile_Ubuntu](../Dockerfile_Ubuntu): Build texconv with GCC and glibc on Ubuntu
- [Dockerfile_Alpine](../Dockerfile_Alpine): Build texconv with GCC and musl on Alpine Linux

## License
Files in this repository (including all submodules) are licensed under [MIT license](../LICENSE).
