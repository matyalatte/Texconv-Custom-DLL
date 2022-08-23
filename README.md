# Texconv-Custom-DLL v0.1.0
Customizable implementation for [Texconv](https://github.com/microsoft/DirectXTex/wiki/Texconv) as a shared library<br>
You can remove some features from Texconv to reduce the file size.

## What's Texconv?
[Texconv](https://github.com/microsoft/DirectXTex/wiki/Texconv)
is a texture converter developed by Microsoft.<br>
It supports all dds formats except for non-2D textures (e.g. cube maps).<br>
There are some dds formats common texture tools can't handle properly, but Texconv can.

## How the dll will be small
It'll be about 20% smaller than the original exe.
- The original exe: 686KB
- The custom dll: 550KB

## Cmake options
There are some cmake options to restore removed features.<br>
You can enable them with `-D` option (e.g. `-D TEXCONV_USE_WIC=ON`).<br>
But it'll make the dll larger.<br>
If you enable all options, the dll will be almost the same size as the original Texconv.<br>
- `TEXCONV_USE_WIC`: Enable to use WIC-supported format (bmp, jpg, png, jxr, etc.)
- `TEXCONV_USE_PPM`: Enable to use .ppm and .pfm
- `TEXCONV_USE_PLANAR`:Enable to use planar dds formats
- `TEXCONV_USE_LOGO`: Enable to use PrintLogo()
- `TEXCONV_USE_USAGE`: Enable to use PrintUsage()
- `TEXCONV_USE_PRINT_INFO`: Enable to use PrintInfo() and PrintFormat()
- `TEXCONV_USE_TIMING`: Enable to use -timing
- `TEXCONV_USE_FLIP`: Enable to use -hflip and -vflip
- `TEXCONV_USE_SWIZZLE`: Enable to use -swizzle
- `TEXCONV_USE_ROTATE_COLOR`: Enable to use -rotatecolor
- `TEXCONV_USE_COLORKEY`: Enable to use -c
- `TEXCONV_USE_ALPHA_CONFIG`: Enable to use -pmalpha, -alpha, -keepcoverage, -aw, and -at
- `TEXCONV_USE_ADDRESSING`: Enable to use -wrap and -mirror
- `TEXCONV_USE_TONEMAP`: Enable to use -tonemap
- `TEXCONV_USE_MULTIPLE_FILES`: Enable to use -r and -flist
- `TEXCONV_USE_NMAP_CONFIG`: Enable to use -nmap and -nmapamp


## Available functions

### texconv
```
int __cdecl texconv(int argc, wchar_t* argv[], bool verbose = true,  bool initCOM = false);
```
It's almost the same as the original texconv.
- `argc`: The number of arguments for texconv.
- `argv`: An array of arguments for texconv.
- `verbose`: Show info
- `initCOM`: Initialize COM

See [the document of texconv](https://github.com/microsoft/DirectXTex/wiki/Texconv) for the arguments.

## Sample code for Python
Here is a sample code for Python.<br>
It'll convert `test.dds` to `outdir/test.png`
```
import ctypes as c
import os

# Get DLL
dll_path = os.path.abspath('Texconv.dll')
dll = c.cdll.LoadLibrary(dll_path)

# Make arguments
dds_file = 'test.dds'
argv = ['-ft', 'png', '-o', 'outdir', '-y', dds_file]
argc = len(args)
argv = [c.c_wchar_p(arg) for arg in argv]
argv = (c.c_wchar_p*len(argv))(*argv)
verbose = c.c_bool(True)
initCOM = c.c_bool(True)

# Convert DDS to PNG
result = dll.texcov(argc, argv, verbose, initCOM)
```
