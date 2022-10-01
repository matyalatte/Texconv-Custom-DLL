# Building Workflow for Ubuntu and MacOS

You can build texconv with Ubuntu + GCC.  
But please note that there are some limitations.  
-   Unable to build as exe.
-   Unable to use GPU for conversion.
-   Unable to make output folder. (It'll raise an error if you specify a directory doesn't exist.)
-   Unable to use some cmake options to restore removed features.

I don't know if it works for other linux distributions.

## 0. Install Clang for macOS

AppleClang doesn't support `std::filesystem` that DirectXTex will use.  
If you want to compile it on mac, you should install Clang with homebrew (`brew install llvm`).

## 1. Get submodules

Move to `./Texconv-Custom-DLL`.  
Then, type `git submodule update --init --recursive`.  
It'll install DirectX related files on the repository.

## 2. Get sal.h
On non-Windows platform, DirectXMath requires [sal.h](https://github.com/dotnet/corert/blob/master/src/Native/inc/unix/sal.h).  
The steps are as follows.

1.  Move to `./Texconv-Custom-DLL/shell_scripts`.
2.  Type `bash get_sal.h`.
3.  It will put the file into `./unix_headers/sal/`.

## 3. Build .so with a shell script

You can build libtexconv.so with a shell script.  
The steps are as follows.

1.  Move to `Texconv-Custom-DLL/shell_scripts`
2.  Type `bash build_on_ubuntu.sh` or `bash build_on_mac.sh` on the terminal. (Do NOT use `sh`!)
3.  `libtexconv.so` will be generated in `Texconv-Custom-DLL/`.
