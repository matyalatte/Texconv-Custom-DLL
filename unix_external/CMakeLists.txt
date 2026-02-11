# additional headers for unix systems
cmake_minimum_required (VERSION 3.14)

# DirectX-Headers
add_subdirectory(DirectX-Headers)

if(NOT WIN32)
    # Download sal.h
    set(SAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sal)
    file(MAKE_DIRECTORY ${SAL_DIR})
    file(DOWNLOAD
        https://raw.githubusercontent.com/dotnet/corert/master/src/Native/inc/unix/sal.h
        ${SAL_DIR}/sal.h
        SHOW_PROGRESS
        EXPECTED_HASH SHA256=bb9ea7c1d05cdfc10a17fe80d87a7cd160c1063c7e5fa089cd6405c31c610a1b
    )
endif()

# DirectXMath
add_subdirectory(DirectXMath)
if(NOT WIN32)
    target_include_directories(DirectXMath INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/sal)
endif()
add_library(Microsoft::DirectXMath ALIAS DirectXMath)

if(NOT WIN32)
    # safestringlib
    add_subdirectory(safestringlib)
endif()
