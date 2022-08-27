# Building Workflow for Windows

## 0. Requirements
- Visual Studio 2022 (need cmake tools)

## 1. Update submodules
Type `git submodule update --init --recursive` to download DirectXTex.

## 2-a. Build .dll with Batch Files
You can build texconv.dll with batch files.<br>
The steps are as follows.

1. Move to `Texconv-Custom-DLL/batch_files`
2. Run `open_x64_native_command_prompt_for_VS2022.bat` to open the command prompt.
3. Run `build.bat` on the command prompt.
4. `texconv.dll` will be generated in `Texconv-Custom-DLL/`.

## 2-b. Build .dll with Visual Studio
Of course, you can build the dll with Visual Studio.<br>
The steps are as follows.

1. Open `./Texconv-Custom-DLL` with Visual Studio.
2. Select Manage Configurations from configuration.
3. Add `-D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded` to `CMake command arguments`.
4. Build texconv.dll.
