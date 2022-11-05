# Texconv-Custom-DLL v0.2.1

Cross-platform and customizable implementation for [Texconv](https://github.com/microsoft/DirectXTex/wiki/Texconv).  
You can remove many features from Texconv to reduce the file size.  
And you can use it as a DLL (or a shared library).  

## What's Texconv?

[Texconv](https://github.com/microsoft/DirectXTex/wiki/Texconv)
is a texture converter developed by Microsoft.  
It supports all dds formats.  
There are some dds formats common tools can't handle properly, but Texconv can.  
Texconv is the best DDS converter as far as I know.  

## How small is the dll?

It's 28% smaller than the original exe.
-   The original exe: 686KB
-   The custom dll: 491KB

If you won't use BC6 and BC7 formats, you can also remove GPU codec with `TEXCONV_NO_GPU_CODEC` option.  
It can be 233KB (66% smaller than the original exe).  

## Removed features

Many features are removed from the original build to reduce the file size.  
-   WIC support (.bmp, .jpg, .png, etc.)
-   3D textures support
-   Little-used dds formats support (B5G6R5, B5G5R5A1, etc.)
-   ppm support (.ppm and .pfm)
-   Print functions
-   Mutilple files processing
-   Many optional arguments

If you want to restore them, you need to use [cmake options](./CMake-Options.md).

## Extra features

You can add some features by [cmake options](./CMake-Options.md).
-   Disable GPU codec (`TEXCONV_NO_GPU_CODEC`)
-   Enable to use [texassemble](https://github.com/microsoft/DirectXTex/wiki/Texassemble) as a DLL function (`TEXCONV_USE_TEXASSEMBLE`)

## Platform

The official texconv only supports Windows.  
But the custom build supports the following platforms and compilers.

-   Windows 10 + MSVC
-   MacOS 10.15 (or later) + AppleClang
-   Ubuntu 20.04 (or later) + GCC

## Download
You can download the built binary from [the release page](https://github.com/matyalatte/Texconv-Custom-DLL/releases).  

-   `TexconvCustomDLL*-Windows.zip` is for Windows.
-   `TexconvCustomDLL*-macOS.tar.bz2` is for Mac (10.15 or later).
-   `TexconvCustomDLL*-Linux.tar.bz2` is for Ubuntu (20.04 or later).

Each zip file has a DLL and a executable.  
You can copy whichever you want to use to your project.  
(The execuatble won't refer the DLL.)  

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

Ubuntu users can also use a docker file to build the binary.  
See [the docker file](../Dockerfile) for the details.  

## License
Files in this repository (including all submodules) are licensed under [MIT license](../LICENSE).
