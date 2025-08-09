# Building Workflow for Windows

## 0. Requirements

-   Visual Studio 2022
-   CMake
-   git

## 1. Get DirectXTex
Move to `./Texconv-Custom-DLL` and type `git submodule update --init --recursive DirectXTex`.  
It downloads DirectXTex to the repository.  
(`git submodule update --init --recursive` works as well, but it downloads unnecessary repositories for Windows.)  

## 2-a. Build .dll with batch files

You can build texconv.dll with batch files.  
Move to `./Texconv-Custom-DLL/batch_files` and type `build.bat` on the command prompt.  
It generates `texconv.dll` in `Texconv-Custom-DLL/`.  

## 2-b. Build .dll with Visual Studio

Of course, you can build the dll with Visual Studio.  
Open `./Texconv-Custom-DLL` with Visual Studio and load CMakeLists.txt.  
Then, you can build texconv.dll.  

## 3. Build executables (optional)

If you want executables, use `build_as_exe.bat` instead of `build.bat`.  
It generates `texconv.exe` and `texassemble.exe` in `./Texconv-Custom-DLL/`.  
You can use them on the command prompt. (e.g. `texconv.exe -ft tga -y -o outdir test.dds`)  

## 4. Build .dll with dynamic linked runtime (optional)

If your app can access to `vcruntime140.dll`, you can use `build_without_vcruntime.bat` to link the dynamic vcruntime to `texconv.dll`.  
The binary size is about 500KB smaller than the static linked dll.
