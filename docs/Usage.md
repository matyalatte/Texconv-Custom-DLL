# Usage

## Executable
You can use `texconv.exe` or `texconv` to convert textures.  

### Arguments

If you want to see the usage of arguments, use the executable with `-help` option. (`texconv.exe -help` or `./texconv -help`)  
And you can see [the document of texconv](https://github.com/microsoft/DirectXTex/wiki/Texconv) for the details.  

### Example

These commands will convert `test.dds` to `outdir/test.tga`.

#### for Windows

```bat
mkdir outdir
texconv.exe -ft tga -o outdir -y test.dds
```

#### for Unix

```shell
mkdir outdir
./texconv -ft tga -o outdir -y test.dds
```

## DLL (or shared library)
You can use texconv as a function of a DLL (or a shared library).

### Available functions

#### texconv
```c++
int texconv(int argc, wchar_t* argv[], bool verbose = true,
            bool init_com = false, bool allow_slow_codec = true,
            wchar_t* err_buf = nullptr, int err_buf_size = 0)
```

-   `argc`: The number of arguments for texconv.
-   `argv`: An array of arguments for texconv.
-   `verbose`: Show info
-   `init_com`: Initialize COM for WIC.
-   `allow_slow_codec`: Allow to use CPU coded for BC6 and BC7.
-   `err_buf`: A buffer to store error messages.
-   `err_buf_size`: The size of the buffer.

The return value is the execution status.  
0 means no errors.  
And 1 means failed to convert.  

#### texassemble

You can use [texassemble](https://github.com/microsoft/DirectXTex/wiki/Texassemble) if you built the dll with `TEXCONV_USE_TEXASSEMBLE` option.

```c++
int texassemble(int argc, wchar_t* argv[], bool verbose = true,
            bool init_com = false,
            wchar_t* err_buf = nullptr, int err_buf_size = 0)
```

-   `argc`: The number of arguments for texassemble.
-   `argv`: An array of arguments for texassemble.
-   `verbose`: Show info
-   `init_com`: Initialize COM for WIC.
-   `err_buf`: A buffer to store error messages.
-   `err_buf_size`: The size of the buffer.

The return value is the execution status.  
0 means no errors.  
And 1 means failed to convert.  

### Example for Python

Here is a sample code for Python.  
It'll convert `test.dds` to `outdir/test.tga`  

```python
import ctypes
import os

# Get DLL
dll_path = os.path.abspath('texconv.dll')  # for windows
# dll_path = os.path.abspath('libtexconv.so')  # for linux
# dll_path = os.path.abspath('libtexconv.dylib')  # for mac
dll = ctypes.cdll.LoadLibrary(dll_path)

# Make arguments
dds_file = 'test.dds'
argv = ['-ft', 'tga', '-o', 'outdir', '-y', dds_file]
argv = [ctypes.c_wchar_p(arg) for arg in argv]
argv = (ctypes.c_wchar_p*len(argv))(*argv)
err_buf = ctypes.create_unicode_buffer(512)

# Convert DDS to TGA
result = dll.texconv(len(argv), argv, verbose=True, init_com=False, allow_slow_codec=False,
                     err_buf=err_buf, err_buf_size=512)

# Show error message
if result != 0:
    raise RuntimeError(err_buf.value)
```
