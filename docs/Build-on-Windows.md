# Building Workflow for Windows

## 0. Requirements

-   Visual Studio 2022
-   CMake

## 1. Get DirectXTex
Move to `./Texconv-Custom-DLL` and type `git submodule update --init --recursive DirectXTex`.  
It'll download DirectXTex to the repository.  
(`git submodule update --init --recursive` will work as well, but it'll download unnecessary repositories for Windows.)  

## 2-a. Build .dll with Batch Files

You can build texconv.dll with batch files.  
Move to `./Texconv-Custom-DLL/batch_files` and type `build.bat` on the command prompt.  
`texconv.dll` will be generated in `Texconv-Custom-DLL/`.  

## 2-b. Build .dll with Visual Studio

Of course, you can build the dll with Visual Studio.  
Open `./Texconv-Custom-DLL` with Visual Studio and load CMakeLists.txt.  
Then, you can build texconv.dll.  

## 3. Build executable

If you want an executable, use `build_as_exe.bat` instead of `build.bat`.  
`texconv.exe` will be generated in `./Texconv-Custom-DLL/`.  
You can use it on the command prompt. (e.g. `texconv.exe -ft tga -y -o outdir test.dds`)  
