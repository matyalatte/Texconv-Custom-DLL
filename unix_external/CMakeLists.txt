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
# it needs sal.h in Texconv-Custom-DLL/unix_external/sal/ (https://github.com/dotnet/corert/blob/master/src/Native/inc/unix/sal.h)
add_library(DirectXMath INTERFACE)
target_include_directories(DirectXMath INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/DirectXMath/Inc
  ${CMAKE_CURRENT_SOURCE_DIR}/sal
  )
target_compile_features(DirectXMath INTERFACE cxx_std_11)

add_library(Microsoft::DirectXMath ALIAS DirectXMath)

# safestringlib
file(GLOB SAFELIB_SOURCES safestringlib/safeclib/*.c)

add_library(safestring_static OBJECT ${SAFELIB_SOURCES})
target_include_directories(safestring_static PUBLIC safestringlib/include)
set_target_properties(safestring_static
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(safestring_static PRIVATE -DSTDC_HEADERS)

target_compile_options(safestring_static
                       PRIVATE -Wno-unknown-pragmas -Wno-unused-parameter)
if(CMAKE_COMPILER_IS_GNUCC AND CMAKE_C_COMPILER_VERSION VERSION_GREATER 7)
  target_compile_options(safestring_static
                         PRIVATE -Wno-implicit-fallthrough)
endif()
target_compile_options(safestring_static
                       PRIVATE -Wall -Wextra -Wsign-compare -Wformat-security)
target_compile_options(safestring_static
                       PRIVATE  -Wstack-protector -Winit-self)
target_compile_options(safestring_static
                       PRIVATE -D_FORTIFY_SOURCE=2 -O2)
target_compile_options(safestring_static
                       PRIVATE -fstack-protector-all)
target_compile_options(safestring_static
                       PRIVATE --param ssp-buffer-size=4 -ftrapv)
target_compile_options(safestring_static PRIVATE -fPIE -fPIC)

if(CMAKE_COMPILER_IS_GNUCC AND
   CMAKE_C_COMPILER_VERSION VERSION_GREATER 6 AND
   CMAKE_C_COMPILER_VERSION VERSION_LESS 9)
    target_compile_options(safestring_static PRIVATE -mmitigate-rop)
endif()

target_compile_options(safestring_static PRIVATE $<$<CONFIG:RELEASE>:-s>)
set_target_properties(safestring_static PROPERTIES LINKER_LANGUAGE C)
