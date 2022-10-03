# Building workflow for ubuntu 20.04 or later.
#
# 1. Use this docker file to build the executable.
#    docker build -t texconv ./
# 2. Run the built image. 
#    docker run texconv
# 3. Use "docker cp" to get the built executable.
#    docker cp <CONTAINER ID>:/Texconv-Custom-DLL/texconv /<SOMEPATH>/texconv
# 4. Also, you can get the built shared library.
#    docker cp <CONTAINER ID>:/Texconv-Custom-DLL/libtexconv.so /<SOMEPATH>/libtexconv.so

# Base image
ARG ubuntu_version="20.04"
FROM ubuntu:${ubuntu_version}

# Install packages
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
       wget ca-certificates build-essential git cmake \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Clone repo
RUN git clone -b dev --recursive --depth=1 https://github.com/matyalatte/Texconv-Custom-DLL.git;\
    bash Texconv-Custom-DLL/shell_scripts/get_sal.sh

# Build shared lib
WORKDIR /Texconv-Custom-DLL/build
RUN cmake \
      -D CMAKE_BUILD_TYPE=Release\
      -D BUILD_DX11=OFF\
      -D BUILD_DX12=OFF\
      -D CMAKE_POSITION_INDEPENDENT_CODE=ON\
      ../;\
    cmake --build .;\
    cp bin/CMake/libtexconv.* ../

# Build executable
RUN cmake \
      -D CMAKE_BUILD_TYPE=Release\
      -D BUILD_DX11=OFF\
      -D BUILD_DX12=OFF\
      -D CMAKE_POSITION_INDEPENDENT_CODE=ON\
      -D TEXCONV_BUILD_AS_EXE=ON\
      -D TEXCONV_USE_USAGE=ON\
      ../;\
    cmake --build .;\
    cp bin/CMake/texconv ../
