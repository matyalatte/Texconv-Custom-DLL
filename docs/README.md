# Texconv-Custom-DLL v0.1.2

Cross-platform and customizable implementation for [Texconv](https://github.com/microsoft/DirectXTex/wiki/Texconv).  
You can remove many features from Texconv to reduce the file size.  
And you can use it as a DLL (or a shared library).  

## What's Texconv?

[Texconv](https://github.com/microsoft/DirectXTex/wiki/Texconv)
is a texture converter developed by Microsoft.  
It supports all dds formats.  
There are some dds formats common tools can't handle properly, but Texconv can.

## How small is the dll?

It's 29% smaller than the original exe.
-   The original exe: 686KB
-   The custom dll: 487KB

## Platform

The official texconv only supports Windows.  
But my implementation supports the following platforms and compilers.

-   Windows 10 + MSVC
-   MacOS 10.15 (or later) + AppleClang
-   Ubuntu 20.04 (or later) + GCC

## Available functions

### texconv

```
int texconv(int argc, wchar_t* argv[], bool verbose = true,  bool initCOM = false);
```

It's almost the same as the original texconv.
-   `argc`: The number of arguments for texconv.
-   `argv`: An array of arguments for texconv.
-   `verbose`: Show info
-   `initCOM`: Initialize COM for WIC.

See [the document of texconv](https://github.com/microsoft/DirectXTex/wiki/Texconv) for the arguments.

## Sample code for Python

Here is a sample code for Python.  
It'll convert `test.dds` to `outdir/test.tga`

```
import ctypes as c
import os

# Get DLL
dll_path = os.path.abspath('texconv.dll')  # for windows
# dll_path = os.path.abspath('libtexconv.so')  # for linux
# dll_path = os.path.abspath('libtexconv.dylib')  # for mac
dll = c.cdll.LoadLibrary(dll_path)

# Make arguments
dds_file = 'test.dds'
argv = ['-ft', 'tga', '-o', 'outdir', '-y', dds_file]
argv = [c.c_wchar_p(arg) for arg in argv]
argv = (c.c_wchar_p*len(argv))(*argv)
argc = len(argv)

# Convert DDS to TGA
result = dll.texconv(argc, argv, verbose=True, initCOM=False)
```

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
