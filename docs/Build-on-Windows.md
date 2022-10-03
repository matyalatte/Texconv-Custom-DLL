# Building Workflow for Windows

## 0. Requirements

-   Visual Studio 2022
-   CMake

## 1. Get DirectXTex
Move to `./Texconv-Custom-DLL`.  
Then, type `git submodule update --init --recursive DirectXTex` to download DirectXTex.  
(`git submodule update --init --recursive` will work as well, but it'll downlaod unnecessary repositories for Windows.)  

## 2-a. Build .dll with Batch Files

You can build texconv.dll with batch files.  
The steps are as follows.

1.  Move to `./Texconv-Custom-DLL/batch_files`
2.  Run `open_x64_native_command_prompt_for_VS2022.bat` to open the command prompt.
3.  Run `build.bat` on the command prompt.
4.  `texconv.dll` will be generated in `Texconv-Custom-DLL/`.

## 2-b. Build .dll with Visual Studio

Of course, you can build the dll with Visual Studio.  
The steps are as follows.

1.  Open `./Texconv-Custom-DLL` with Visual Studio.
2.  Select Manage Configurations from configuration.
3.  Add `-D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded` to `CMake command arguments`.
4.  Build texconv.dll.

## 3. Build executable

If you want an executable, use `build_as_exe.bat` instead of `build.bat`.  
`texconv.exe` will be generated in `./Texconv-Custom-DLL/`.  
You can use it on the terminal like `texconv.exe -ft tga -y -o outdir test.dds`.  
