# CMake Options

There are many cmake options to customize the binaries.  
You can enable them with `-D` option (e.g. `-D TEXCONV_USE_TEXASSEMBLE=ON`).  

## Exe options

-   `TEXCONV_BUILD_AS_EXE`: Build Texconv as .exe.
-   `TEXCONV_ICON_FOR_EXE`: Use `directx.ico` when `TEXCONV_BUILD_AS_EXE` is ON.

## Other options

-   `TEXCONV_NO_GPU_CODEC`: Disable GPU codec for BC6 and BC7 (Only available for Windows)
-   `TEXCONV_USE_TEXASSEMBLE`: Enable to use texassemble as a DLL function
