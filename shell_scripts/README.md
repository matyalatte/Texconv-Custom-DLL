# Building Workflow for Ubuntu and macOS
You can build texconv with Ubuntu + GCC.<br>
But please note that there are some limitations.<br>
- Unable to build as exe.
- Unable to use GPU for conversion.
- Unable to make output folder. (It'll raise an error if you specify a directory doesn't exist.)
- Unable to use some cmake options to restore removed features.

I don't know if it works for other linux distributions.

## 0. Install Clang for macOS.
AppleClang doesn't support `std::filesystem` that DirectXTex will use.<br>
If you want to compile it on mac, you should install Clang with homebrew (`brew install llvm`).

## 1. Install DirectX-Headers
Clone, build, and install [DirectX-Headers](https://github.com/microsoft/DirectX-Headers).<br>
like this

```
git clone https://github.com/microsoft/DirectX-Headers
cd DirectX-Headers
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release -D DXHEADERS_BUILD_TEST=OFF -D DXHEADERS_BUILD_GOOGLE_TEST=OFF ../
sudo make install
```

## 2. Clone DirectXMath
Clone [DirectXMath](https://github.com/microsoft/DirectXMath).

## 3. Get `sal.h`
Linux need `sal.h` for DirectXMath.<br>
Move to `DirectXMath/Inc`.<br>
Then, type `wget https://raw.githubusercontent.com/dotnet/corert/master/src/Native/inc/unix/sal.h`

## 4. Add `sal.h` to library
Open `DirectXMath/CMakeLists.txt`.<br>
Then, add `sal.h` to the library.

```
set(LIBRARY_HEADERS
    Inc/DirectXCollision.h
    Inc/DirectXCollision.inl
    Inc/DirectXColors.h
    Inc/DirectXMath.h
    Inc/DirectXMathConvert.inl
    Inc/DirectXMathMatrix.inl
    Inc/DirectXMathMisc.inl
    Inc/DirectXMathVector.inl
    Inc/DirectXPackedVector.h
    Inc/DirectXPackedVector.inl
    Inc/sal.h)
```

## 5. Build DirectXMath
Move to `DirectXMath`.<br>
Then, build DirectXMath.

```
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release ../
sudo make install
```

## 6. Download safeclib
```
wget https://github.com/rurban/safeclib/releases/download/v3.7.1/safeclib-3.7.1.tar.bz2
tar -xvjof safeclib-3.7.1.tar.bz2
rm -f safeclib-3.7.1.tar.bz2
cd safaclib-3.7.1
mkdir build
cd build
../configure
sudo make install
```
## 6. Get submodules for Texconv
Move to `Texconv-Custom-DLL`.
Then, type `git submodule update --init --recursive` to download DirectXTex.

## 7. Build .so with a shell script
You can build libtexconv.so with a shell script.<br>
The steps are as follows.

1. Move to `Texconv-Custom-DLL/shell_scripts`
2. Type `bash build_on_"your os".sh` on the terminal. (Do NOT use `sh`!)
3. `libtexconv.so` will be generated in `Texconv-Custom-DLL/`.

