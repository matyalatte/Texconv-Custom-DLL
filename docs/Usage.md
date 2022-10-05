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

### Available function

```c++
int texconv(int argc, wchar_t* argv[], bool verbose = true,  bool initCOM = false)
```

-   `argc`: The number of arguments for texconv.
-   `argv`: An array of arguments for texconv.
-   `verbose`: Show info
-   `initCOM`: Initialize COM for WIC.

The return value is the execution status.  
0 means no errors.  
And 1 means failed to convert.  

### Example for Python

Here is a sample code for Python.  
It'll convert `test.dds` to `outdir/test.tga`  

```python
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

# Convert DDS to TGA
result = dll.texconv(len(argv), argv, verbose=True, initCOM=False)
```