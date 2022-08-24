# Building Workflow for Windows

## 0. Requirements
- Visual Studio 2022 (need cmake tools)

## 1-a. Build .exe with Batch Files
You can build texconv.dll with batch files.<br>
The steps are as follows.

1. Run `open_x64_native_command_prompt_for_VS2022.bat` to open the command prompt.
2. Run `build.bat` on the command prompt.
3. `texconv.dll` will be generated in `Texconv-Custom-DLL/`.

## 1-b. Build .exe with Visual Studio
Of course, you can build the dll with Visual Studio.<br>
The steps are as follows.

1. Open `./Texconv-Custom-DLL` with Visual Studio.
2. Select Manage Configurations from configuration.
3. Add `-D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded` to `CMake command arguments`.
4. Build texconv.dll.
