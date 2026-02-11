# Texconv-Custom-DLL v0.5.0

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

You can download built binaries from [the release page](https://github.com/matyalatte/Texconv-Custom-DLL/releases).  
Each zip file has a DLL and executables. The executables don't refer the DLL.

-   `TexconvCustomDLL*-Windows.zip` is for Windows.
-   `TexconvCustomDLL*-macOS.tar.bz2` is for macOS (10.15 or later).
-   `TexconvCustomDLL*-Linux.tar.bz2` is for Linux with GLIBC 2.27+ and GLIBCXX 3.4.26+.

> [!Note]
> The linux build only supports distributions using GLIBC.  
> Build the executable by yourself if you want to use it on unsupported distros.

## Usage

See here if you want to know how to use texconv.  
[Usage](./Usage.md)  

## Building

### Windows

There is a document for Windows users.  
[Building Workflow for Windows](./Build-on-Windows.md)  

### Linux and Mac

There is a document for Linux and Mac users.  
[Building Workflow for Linux and MacOS](./Build-on-Unix.md)  

### Docker

Linux users can use docker files to build the binary.  
See the files for the details.  

- [Dockerfile_Ubuntu](../Dockerfile_Ubuntu): Build texconv with GCC and glibc on Ubuntu
- [Dockerfile_Alpine](../Dockerfile_Alpine): Build texconv with GCC and musl on Alpine Linux

## Licenses

The original source code of this repository is licensed
under the MIT License (see [LICENSE](../LICENSE)).

This project includes third-party software, which is licensed under its own
respective licenses. When redistributing binaries, you must retain
[THIRD_PARTY_LICENSES.txt](../THIRD_PARTY_LICENSES.txt) to comply with those
licenses.

See [THIRD_PARTY_LICENSES.txt](../THIRD_PARTY_LICENSES.txt) for full license texts
and copyright notices.

### Third-party components

| Project | License | Included in | Purpose |
|---------|---------|-------------| ------- |
| [DirectXTex](https://github.com/microsoft/DirectXTex) | MIT | All platforms ||
| [DirectX-Headers](https://github.com/microsoft/DirectX-Headers) | MIT | All platforms ||
| [DirectXMath](https://github.com/microsoft/DirectXMath) | MIT | All platforms ||
| [safestringlib](https://github.com/intel/safestringlib) | MIT | Linux / macOS ||
| [libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo) | zlib/libpng, IJG | Linux / macOS | JPEG support |
| [libpng](https://github.com/pnggroup/libpng) | zlib/libpng | Linux / macOS | PNG support |
| [zlib](https://github.com/madler/zlib.git) | zlib/libpng | Linux | PNG support |
| [OpenEXR](https://github.com/AcademySoftwareFoundation/openexr) | BSD-3 | All platforms | EXR support |
| [Imath](https://github.com/AcademySoftwareFoundation/Imath) | BSD-3 | All platforms | EXR support |
| [libdeflate](https://github.com/ebiggers/libdeflate) | MIT | All platforms | EXR support |
| [OpenJPH](https://github.com/aous72/OpenJPH) | BSD-2 | All platforms | EXR support |
