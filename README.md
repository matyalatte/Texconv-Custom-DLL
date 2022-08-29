# Texconv-Custom-DLL v0.1.1
Customizable implementation for [Texconv](https://github.com/microsoft/DirectXTex/wiki/Texconv).<br>
You can remove many features from Texconv to reduce the file size.<br>
And you can use it as a DLL.

## What's Texconv?
[Texconv](https://github.com/microsoft/DirectXTex/wiki/Texconv)
is a texture converter developed by Microsoft.<br>
It supports all dds formats.<br>
There are some dds formats common tools can't handle properly, but Texconv can.

## How small is the dll?
It's 29% smaller than the original exe.
- The original exe: 686KB
- The custom dll: 487KB

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
It'll convert `test.dds` to `outdir/test.tga`
```
import ctypes as c
import os

# Get DLL
dll_path = os.path.abspath('texconv.dll')
dll = c.cdll.LoadLibrary(dll_path)

# Make arguments
dds_file = 'test.dds'
argv = ['-ft', 'tga', '-o', 'outdir', '-y', dds_file]
argc = len(argv)
argv = [c.c_wchar_p(arg) for arg in argv]
argv = (c.c_wchar_p*len(argv))(*argv)
verbose = c.c_bool(True)
initCOM = c.c_bool(True)

# Convert DDS to TGA
result = dll.texconv(argc, argv, verbose, initCOM)
```

## Removed features
Many features are removed from the original build to reduce the file size.<br>
- WIC support (.bmp, .jpg, .png, etc.)
- 3D textures support
- Little-used dds formats support (B5G6R5, B5G5R5A1, etc.)
- ppm support (.ppm and .pfm)
- Print functions
- Mutilple files processing
- Many optional arguments

If you want to restore them, you need to use cmake options.

## Cmake options
There are many cmake options to restore removed features.<br>
You can enable them with `-D` option (e.g. `-D TEXCONV_USE_WIC=ON`).<br>
But it'll make the dll larger.<br>
If you enable all options, the dll will be almost the same size as the original Texconv.<br>

#### exe options
- `TEXCONV_BUILD_AS_EXE`: Build Texconv as .exe
- `TEXCONV_USE_ICON_FOR_EXE`: Use `directx.ico` when TEXCONV_BUILD_AS_EXE is ON

#### format options
- `TEXCONV_USE_WIC`: Enable to use WIC-supported format (bmp, jpg, png, jxr, etc.)
- `TEXCONV_USE_PPM`: Enable to use .ppm and .pfm
- `TEXCONV_USE_PLANAR`: Enable to use planar dds formats
- `TEXCONV_USE_3D`: Enable to use 3D textures
- `TEXCONV_USE_LEGACY`: Enable to use legacy dds formats
- `TEXCONV_USE_16BPP`: Enable to use 16bpp format (B5G6R5, B5G5R5A1, B4G4R4A4)

#### Printing options
- `TEXCONV_USE_LOGO`: Enable to use PrintLogo()
- `TEXCONV_USE_USAGE`: Enable to use -help
- `TEXCONV_USE_PRINT_INFO`: Enable to use PrintInfo() and PrintFormat()
- `TEXCONV_USE_TIMING`: Enable to use -timing

#### Other options
- `TEXCONV_USE_FLIP`: Enable to use -hflip and -vflip
- `TEXCONV_USE_SWIZZLE`: Enable to use -swizzle
- `TEXCONV_USE_ROTATE_COLOR`: Enable to use -rotatecolor
- `TEXCONV_USE_COLORKEY`: Enable to use -c
- `TEXCONV_USE_ALPHA_CONFIG`: Enable to use -pmalpha, -alpha, -keepcoverage, -aw, and -at
- `TEXCONV_USE_ADDRESSING`: Enable to use -wrap and -mirror
- `TEXCONV_USE_TONEMAP`: Enable to use -tonemap
- `TEXCONV_USE_MULTIPLE_FILES`: Enable to use -r and -flist
- `TEXCONV_USE_NMAP_CONFIG`: Enable to use -nmap and -nmapamp
- `TEXCONV_USE_MINOR_DDS_CONFIG`: Enable to use -tu, -tf, -dword, -badtails, -fixbc4x4, -xlum, -dx10, and -dx9
- `TEXCONV_USE_FEATURE_LEVEL`: Enable to use -fl
- `TEXCONV_USE_BC_CONFIG`: Enable to use -bc
- `TEXCONV_USE_SRGB`: Enable to use -srgb, -srgbi, and -srgbo
- `TEXCONV_USE_NAME_CONFIG`: Enable to use -px, -sx, and -l
- `TEXCONV_USE_DITHER`: Enable to use dither filters
- `TEXCONV_USE_GPU_CONFG`: Enable to use -gpu and -nogpu
- `TEXCONV_USE_X2BIAS`: Enable to use -x2bias
- `TEXCONV_USE_SINGLEPROC`: Enable to use -singleproc
- `TEXCONV_USE_TGA20`: Enable to use -tga20
