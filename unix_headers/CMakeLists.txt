# additional headers for unix systems
cmake_minimum_required (VERSION 3.14)

# DirectX-Headers
add_library(DirectX-Headers INTERFACE)
target_include_directories(DirectX-Headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/DirectX-Headers/include)
if (NOT WIN32)
    target_include_directories(DirectX-Headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/DirectX-Headers/include/wsl/stubs)
endif()
add_library(Microsoft::DirectX-Headers ALIAS DirectX-Headers)

# DirectXMath
# it needs sal.h for unix (https://github.com/dotnet/corert/blob/master/src/Native/inc/unix/sal.h)
add_library(DirectXMath INTERFACE)
target_include_directories(DirectXMath INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/DirectXMath/Inc
  ${CMAKE_CURRENT_SOURCE_DIR}/corert/src/Native/inc/unix
  )
target_compile_features(DirectXMath INTERFACE cxx_std_11)

add_library(Microsoft::DirectXMath ALIAS DirectXMath)