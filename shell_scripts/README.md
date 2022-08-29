# Building Workflow for Ubuntu
I don't know if it works for other linux distributions.

## 1. Install DirectX-Headers
Clone, build, and install [DirectX-Headers](https://github.com/microsoft/DirectX-Headers).<br>
like this

```
git clone https://github.com/microsoft/DirectX-Headers
cd DirectX-Headers
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release ../
sudo make install
```

## 2. Install DirectXMath
Install [DirectXMath](https://github.com/microsoft/DirectXMath) in the same way as DirectX-Headers.

## 3. Get submodules for Texconv
Move to `Texconv-Custom-DLL`.
Then, type `git submodule update --init --recursive` to download DirectXTex.

## 4. Build .so with a shell script
You can build libtexconv.so with a shell script.<br>
The steps are as follows.

1. Move to `Texconv-Custom-DLL/shell_scripts`
2. Type `bash build.sh` on the terminal. (Do NOT use `sh`!)
3. `libtexconv.so` will be generated in `Texconv-Custom-DLL/`.

