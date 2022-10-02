# Building Workflow for Ubuntu and MacOS

You can build texconv with Ubuntu and MacOS.  
But please note that there are some limitations.  
-   Unable to build as exe.
-   Unable to use GPU for conversion.
-   Unable to make output folder. (It'll raise an error if you specify a directory doesn't exist.)
-   Unable to use some cmake options to restore removed features.

Also, the offical Texconv only supports Windows and WSL.  
I made sure I can build it with the following platforms and compilers.  

-   Ubuntu 20.04 + GCC 9.4
-   MacOS 10.15 + AppleClang 12.0

But it might not work on your environment.  

## 1. Get submodules

Move to `./Texconv-Custom-DLL`.  
Then, type `git submodule update --init --recursive`.  
It'll install DirectX related files on the repository.

## 2. Get sal.h

On non-Windows platform, DirectXMath requires [sal.h](https://github.com/dotnet/corert/blob/master/src/Native/inc/unix/sal.h).  
Move to `./Texconv-Custom-DLL/shell_scripts` and type `bash get_sal.h`.  
It will put the file into `./unix_external/sal/`.

## 3. Build the binary with a shell script

You can build `libtexconv.so` (or `libtexconv.dylib`) with a shell script.  
Move to `Texconv-Custom-DLL/shell_scripts` and type `bash build.sh` on the terminal.  
`libtexconv.so` (or `libtexconv.dylib`) will be generated in `./Texconv-Custom-DLL/`.
