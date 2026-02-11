# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.20)

set(DIRECTXTEX_VERSION 2.0.9)

project (DirectXTex
  VERSION ${DIRECTXTEX_VERSION}
  DESCRIPTION "DirectX Texture Library"
  HOMEPAGE_URL "https://go.microsoft.com/fwlink/?LinkId=248926"
  LANGUAGES C CXX)

option(BUILD_TOOLS "Build tex command-line tools" ON)

set(BUILD_SAMPLE OFF)

# Includes the functions for Direct3D 11 resources and DirectCompute compression
option(BUILD_DX11 "Build with DirectX11 Runtime support (requires fxc.exe)" ON)

# Includes the functions for creating Direct3D 12 resources at runtime
option(BUILD_DX12 "Build with DirectX12 Runtime support" ON)

# Enable the use of OpenMP for software BC6H/BC7 compression
option(BC_USE_OPENMP "Build with OpenMP support" ON)

option(USE_PREBUILT_SHADERS "Use externally built HLSL shaders" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# setup for mac
if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "" FORCE)
endif()

include(GNUInstallDirs)
include("options.cmake")

#--- Library
set(LIBRARY_HEADERS
    DirectXTex/DirectXTex/DirectXTex.h
    DirectXTex/DirectXTex/DirectXTex.inl)

set(LIBRARY_SOURCES
    DirectXTex/DirectXTex/BC.h
    DirectXTex/DirectXTex/DDS.h
    DirectXTex/DirectXTex/DirectXTexP.h
    DirectXTex/DirectXTex/filters.h
    DirectXTex/DirectXTex/scoped.h
    DirectXTex/DirectXTex/BC.cpp
    DirectXTex/DirectXTex/BC4BC5.cpp
    DirectXTex/DirectXTex/BC6HBC7.cpp
    DirectXTex/DirectXTex/DirectXTexCompress.cpp
    DirectXTex/DirectXTex/DirectXTexConvert.cpp
    DirectXTex/DirectXTex/DirectXTexDDS.cpp
    DirectXTex/DirectXTex/DirectXTexHDR.cpp
    DirectXTex/DirectXTex/DirectXTexImage.cpp
    DirectXTex/DirectXTex/DirectXTexMipmaps.cpp
    DirectXTex/DirectXTex/DirectXTexMisc.cpp
    DirectXTex/DirectXTex/DirectXTexNormalMaps.cpp
    DirectXTex/DirectXTex/DirectXTexPMAlpha.cpp
    DirectXTex/DirectXTex/DirectXTexResize.cpp
    DirectXTex/DirectXTex/DirectXTexTGA.cpp
    DirectXTex/DirectXTex/DirectXTexUtil.cpp)

if(WIN32)
   list(APPEND LIBRARY_SOURCES
       DirectXTex/DirectXTex/DirectXTexFlipRotate.cpp
       DirectXTex/DirectXTex/DirectXTexWIC.cpp)
endif()

set(SHADER_SOURCES
    DirectXTex/DirectXTex/Shaders/BC6HEncode.hlsl
    DirectXTex/DirectXTex/Shaders/BC7Encode.hlsl)

if(BUILD_DX11 AND WIN32)
   list(APPEND LIBRARY_SOURCES
       DirectXTex/DirectXTex/BCDirectCompute.h
       DirectXTex/DirectXTex/BCDirectCompute.cpp
       DirectXTex/DirectXTex/DirectXTexCompressGPU.cpp
       DirectXTex/DirectXTex/DirectXTexD3D11.cpp)
endif()

if(BUILD_DX12)
   list(APPEND LIBRARY_SOURCES DirectXTex/DirectXTex/DirectXTexD3D12.cpp)

   if(NOT (DEFINED XBOX_CONSOLE_TARGET))
       list(APPEND LIBRARY_SOURCES DirectXTex/Common/d3dx12.h)
   endif()
endif()

if(ENABLE_OPENEXR_SUPPORT)
   list(APPEND LIBRARY_HEADERS DirectXTex/Auxiliary/DirectXTexEXR.h)
   list(APPEND LIBRARY_SOURCES DirectXTex/Auxiliary/DirectXTexEXR.cpp)
endif()

if(ENABLE_LIBJPEG_SUPPORT)
    if(WIN32)
        message(STATUS "Use of the Windows Imaging Component (WIC) instead of libjpeg is recommended.")
    endif()
    list(APPEND LIBRARY_HEADERS DirectXTex/Auxiliary/DirectXTexJPEG.h)
    list(APPEND LIBRARY_SOURCES DirectXTex/Auxiliary/DirectXTexJPEG.cpp)
endif()

if(ENABLE_LIBPNG_SUPPORT)
    if(WIN32)
        message(STATUS "Use of the Windows Imaging Component (WIC) instead of libpng is recommended.")
    endif()
    list(APPEND LIBRARY_HEADERS DirectXTex/Auxiliary/DirectXTexPNG.h)
    list(APPEND LIBRARY_SOURCES DirectXTex/Auxiliary/DirectXTexPNG.cpp)
endif()

if(ENABLE_OPENEXR_SUPPORT)
  if(TEXCONV_USE_STATIC_LINK)
    include(FetchContent)
    set(BUILD_SHARED_LIBS      OFF CACHE BOOL "" FORCE)
    set(OPENEXR_INSTALL        OFF CACHE BOOL "" FORCE)
    set(OPENEXR_BUILD_TOOLS    OFF CACHE BOOL "" FORCE)
    set(OPENEXR_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(OPENEXR_TEST_LIBRARIES OFF CACHE BOOL "" FORCE)
    set(OPENEXR_FORCE_INTERNAL_DEFLATE ON CACHE BOOL "" FORCE)
    set(OPENEXR_FORCE_INTERNAL_OPENJPH ON CACHE BOOL "" FORCE)
    set(OPENEXR_FORCE_INTERNAL_IMATH   ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
        openexr
        GIT_REPOSITORY https://github.com/AcademySoftwareFoundation/openexr.git
        GIT_TAG v3.4.4
    )
    FetchContent_MakeAvailable(openexr)
  else()
    find_package(OpenEXR REQUIRED)
  endif()
endif()

# Note: CompilerAndLinker.cmake should be called after getting OpenEXR::OpenEXR
include(DirectXTex/build/CompilerAndLinker.cmake)

if(BUILD_DX11 AND WIN32)
    if(NOT COMPILED_SHADERS)
        if(USE_PREBUILT_SHADERS)
            message(FATAL_ERROR "ERROR: Using prebuilt shaders requires the COMPILED_SHADERS variable is set")
        endif()
        set(COMPILED_SHADERS ${CMAKE_CURRENT_BINARY_DIR}/Shaders/Compiled)
        file(MAKE_DIRECTORY ${COMPILED_SHADERS})
    else()
        file(TO_CMAKE_PATH ${COMPILED_SHADERS} COMPILED_SHADERS)
    endif()

    list(APPEND LIBRARY_SOURCES ${COMPILED_SHADERS}/BC6HEncode_EncodeBlockCS.inc)

    if(NOT USE_PREBUILT_SHADERS)
        find_program(DIRECTX_FXC_TOOL FXC.EXE
          HINTS "C:/Program Files (x86)/Windows Kits/10/bin/${CMAKE_SYSTEM_VERSION}.0/${DIRECTX_HOST_ARCH}"
                "C:/Program Files (x86)/Windows Kits/10/bin/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}/${DIRECTX_HOST_ARCH}")
        message(STATUS "Using LegacyShaderCompiler found in ${DIRECTX_FXC_TOOL}")
        add_custom_command(
            OUTPUT "${COMPILED_SHADERS}/BC6HEncode_EncodeBlockCS.inc"
            MAIN_DEPENDENCY "${PROJECT_SOURCE_DIR}/DirectXTex/DirectXTex/Shaders/CompileShaders.cmd"
            DEPENDS ${SHADER_SOURCES}
            COMMENT "Generating HLSL shaders..."
            COMMAND ${CMAKE_COMMAND} -E env CompileShadersOutput="${COMPILED_SHADERS}" $<$<BOOL:${DIRECTX_FXC_TOOL}>:LegacyShaderCompiler=${DIRECTX_FXC_TOOL}> CompileShaders.cmd > "${COMPILED_SHADERS}/compileshaders.log"
            WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/DirectXTex/DirectXTex/Shaders"
            USES_TERMINAL)
    endif()
endif()

add_library(${PROJECT_NAME} STATIC ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})

if(BUILD_DX11 AND WIN32)
   target_include_directories(${PROJECT_NAME} PRIVATE ${COMPILED_SHADERS})
endif()

source_group(${PROJECT_NAME} REGULAR_EXPRESSION DirectXTex/DirectXTex/*.*)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR}
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DirectXTex/DirectXTex>)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

if(BC_USE_OPENMP)
  find_package(OpenMP)
  if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
  else()
    set(BC_USE_OPENMP OFF)
  endif()
endif()

if(ENABLE_OPENEXR_SUPPORT)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenEXR::OpenEXR)
endif()

if(ENABLE_LIBJPEG_SUPPORT)
    if(TEXCONV_USE_STATIC_LINK)
        # Build static binary as PIC
        include(ExternalProject)

        set(JPEG_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libjpeg-turbo-install)
        set(JPEG_LIB_DIR ${JPEG_INSTALL_PREFIX}/lib)
        set(JPEG_INCLUDE_DIR ${JPEG_INSTALL_PREFIX}/include)

        # Note: libjpeg-turbo does not support FetchContent_Declare().
        ExternalProject_Add(
            libjpeg-turbo
            GIT_REPOSITORY https://github.com/libjpeg-turbo/libjpeg-turbo.git
            GIT_TAG 3.1.3
            CMAKE_ARGS
                -DCMAKE_INSTALL_PREFIX=${JPEG_INSTALL_PREFIX}
                -DENABLE_SHARED=OFF
                -DENABLE_STATIC=ON
                -DCMAKE_POSITION_INDEPENDENT_CODE=ON
                -DCMAKE_C_VISIBILITY_PRESET=hidden
                -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
                -DCMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}
                -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
                -DWITH_TURBOJPEG=OFF
                -DWITH_TOOLS=OFF
                -DWITH_TESTS=OFF
            BUILD_BYPRODUCTS
                ${JPEG_LIB_DIR}/libjpeg.a
        )
        ExternalProject_Get_Property(libjpeg-turbo BINARY_DIR SOURCE_DIR)
        add_library(libjpeg_turbo_static STATIC IMPORTED)
        file(MAKE_DIRECTORY ${JPEG_INCLUDE_DIR})
        target_include_directories(libjpeg_turbo_static INTERFACE ${JPEG_INCLUDE_DIR})
        set_target_properties(libjpeg_turbo_static PROPERTIES IMPORTED_LOCATION ${JPEG_LIB_DIR}/libjpeg.a)
        add_dependencies(libjpeg_turbo_static libjpeg-turbo)
        add_library(JPEG::JPEG ALIAS libjpeg_turbo_static)
    else()
        find_package(JPEG REQUIRED)
    endif()
    target_link_libraries(${PROJECT_NAME} PUBLIC JPEG::JPEG)
endif()

if(ENABLE_LIBPNG_SUPPORT)
    if(TEXCONV_USE_STATIC_LINK)
        include(FetchContent)
        # Note: zlib is pre-installed on macOS.
        if(NOT APPLE)
            set(ZLIB_BUILD_SHARED   OFF CACHE BOOL "" FORCE)
            set(ZLIB_BUILD_STATIC   ON  CACHE BOOL "" FORCE)
            set(ZLIB_BUILD_TESTING  OFF CACHE BOOL "" FORCE)

            FetchContent_Declare(
                zlib
                GIT_REPOSITORY https://github.com/madler/zlib.git
                GIT_TAG v1.3.1.2
            )
            FetchContent_MakeAvailable(zlib)
            add_library(ZLIB::ZLIB ALIAS zlibstatic)
            set(ZLIB_FOUND TRUE)
            set(ZLIB_INCLUDE_DIR  "${zlib_SOURCE_DIR}")
            set(ZLIB_INCLUDE_DIRS "${zlib_SOURCE_DIR}")
            set(ZLIB_LIBRARY zlibstatic)
            set(ZLIB_LIBRARIES zlibstatic)
            set(PNG_USE_SYSTEM_ZLIB OFF CACHE BOOL "" FORCE)
        endif()

        set(PNG_SHARED OFF CACHE BOOL "" FORCE)
        set(PNG_TESTS  OFF CACHE BOOL "" FORCE)
        set(PNG_TOOLS  OFF CACHE BOOL "" FORCE)

        FetchContent_Declare(
            libpng
            GIT_REPOSITORY https://github.com/pnggroup/libpng.git
            GIT_TAG v1.6.54
        )

        FetchContent_MakeAvailable(libpng)
        add_library(PNG::PNG ALIAS png_static)
    else()
        find_package(PNG REQUIRED)
    endif()
    target_link_libraries(${PROJECT_NAME} PUBLIC PNG::PNG)
endif()

if(NOT MINGW)
    target_precompile_headers(${PROJECT_NAME} PRIVATE DirectXTex/DirectXTex/DirectXTexP.h)
endif()

if(WIN32)
    find_package(directxmath CONFIG QUIET)
    find_package(directx-headers CONFIG QUIET)
    if(MINGW)
        if(NOT directxmath_FOUND)
            add_subdirectory(unix_external/DirectXMath)
            add_library(Microsoft::DirectXMath ALIAS DirectXMath)
            set(directxmath_FOUND ON)
        endif()
        if(NOT directx-headers_FOUND)
            find_package(directx-headers CONFIG QUIET)
            add_subdirectory(unix_external/DirectX-Headers)
            set(directx-headers_FOUND ON)
        endif()
    endif()
else()
    message("INFO: Using a subdirectory 'unix_external' for DirectX-Headers, DirectXMath, and safestringlib.")
    add_subdirectory(unix_external)
endif()

if(directxmath_FOUND OR (NOT WIN32))
    message(STATUS "Using DirectXMath package")
    target_link_libraries(${PROJECT_NAME} PUBLIC Microsoft::DirectXMath)
endif()

if(directx-headers_FOUND OR (NOT WIN32))
    message(STATUS "Using DirectX-Headers package")
    target_link_libraries(${PROJECT_NAME} PUBLIC Microsoft::DirectX-Headers)
    target_compile_definitions(${PROJECT_NAME} PUBLIC USING_DIRECTX_HEADERS)
endif()

#--- Command-line tools
set(TOOL_EXES "")

if (BUILD_TOOLS)
  configure_file(${CMAKE_SOURCE_DIR}/TexconvDLL/config.h.in ${CMAKE_BINARY_DIR}/TexconvDLL/config.h @ONLY)
endif()

if (BUILD_TOOLS AND TEXCONV_USE_TEXASSEMBLE AND TEXCONV_BUILD_AS_EXE)
  set(TEXASSEMBLE_FILES
    ${CMAKE_BINARY_DIR}/TexconvDLL/config.h
    ${CMAKE_SOURCE_DIR}/TexconvDLL/config.h.in
    TexconvDLL/tool_util.h
    TexconvDLL/texassemble.cpp)
  if (WIN32)
    if (TEXCONV_USE_WIC)
      set(TEXASSEMBLE_FILES ${TEXASSEMBLE_FILES}
        DirectXTex/Texassemble/AnimatedGif.cpp)
    endif()
    if (TEXCONV_ICON_FOR_EXE)
      set(TEXASSEMBLE_FILES ${TEXASSEMBLE_FILES}
        TexconvDLL/texconv.rc)
    endif()
    set(TEXASSEMBLE_FILES ${TEXASSEMBLE_FILES}
      DirectXTex/Common/settings.manifest)
  endif()
    add_executable(texassemble ${TEXASSEMBLE_FILES})
  target_compile_features(texassemble PRIVATE cxx_std_17)
  if (WIN32)
    target_link_libraries(texassemble PRIVATE ${PROJECT_NAME} ole32.lib version.lib)
  else()
    # unix need a library for safe functions
    target_link_libraries(texassemble PRIVATE ${PROJECT_NAME} safestring_static)
  endif()
  source_group(texassemble FILES ${TEXASSEMBLE_FILES})
  list(APPEND TOOL_EXES texassemble)
endif()

if(BUILD_TOOLS)
  set(TEXCONV_FILES
    ${CMAKE_BINARY_DIR}/TexconvDLL/config.h
    ${CMAKE_SOURCE_DIR}/TexconvDLL/config.h.in
    TexconvDLL/tool_util.h
    TexconvDLL/texconv.cpp)
  if (TEXCONV_USE_TEXASSEMBLE)
    if (NOT TEXCONV_BUILD_AS_EXE)
      set(TEXCONV_FILES ${TEXCONV_FILES}
        TexconvDLL/texassemble.cpp)
      if (TEXCONV_USE_WIC AND WIN32)
        set(TEXCONV_FILES ${TEXCONV_FILES}
          DirectXTex/Texassemble/AnimatedGif.cpp)
      endif()
    endif()
  endif()
  if (TEXCONV_USE_WIC AND WIN32)
    set(TEXCONV_FILES ${TEXCONV_FILES}
      DirectXTex/Texconv/ExtendedBMP.cpp
      DirectXTex/Texconv/PortablePixMap.cpp)
  endif()
  if (TEXCONV_BUILD_AS_EXE)
    if (WIN32)
      if (TEXCONV_ICON_FOR_EXE)
        set(TEXCONV_FILES ${TEXCONV_FILES}
          TexconvDLL/texconv.rc)
      endif()
      set(TEXCONV_FILES ${TEXCONV_FILES}
        DirectXTex/Common/settings.manifest)
    endif()
    add_executable(texconv ${TEXCONV_FILES})
  else()
    add_library(texconv SHARED ${TEXCONV_FILES})
    if (WIN32)
        # output name should be texconv.dll, not libtexconv.dll
        set(CMAKE_SHARED_LIBRARY_PREFIX "")
    endif()
  endif()
  target_compile_features(texconv PRIVATE cxx_std_17)
  if (WIN32)
    target_link_libraries(texconv PRIVATE ${PROJECT_NAME} ole32.lib shell32.lib version.lib)
  else()
    # unix need a library for safe functions
    target_link_libraries(texconv PRIVATE ${PROJECT_NAME} safestring_static)
  endif()
  source_group(texconv FILES ${TEXCONV_FILES})
  list(APPEND TOOL_EXES texconv)
endif()

foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
  target_include_directories(${t} PRIVATE DirectXTex/Common)
endforeach()

if(BUILD_TOOLS)
  if(ENABLE_OPENEXR_SUPPORT)
    foreach(t IN LISTS TOOL_EXES)
      target_include_directories(${t} PRIVATE DirectXTex/Auxiliary)
      target_link_libraries(${t} PRIVATE ${OPENEXR_ILMIMF_LIBRARY})
      target_compile_definitions(${t} PRIVATE USE_OPENEXR)
    endforeach()
  endif()
  if(ENABLE_LIBJPEG_SUPPORT)
    foreach(t IN LISTS TOOL_EXES)
      target_include_directories(${t} PRIVATE DirectXTex/Auxiliary)
      target_link_libraries(${t} PRIVATE JPEG::JPEG)
      target_compile_definitions(${t} PRIVATE USE_LIBJPEG)
    endforeach()
  endif()
  if(ENABLE_LIBPNG_SUPPORT)
    foreach(t IN LISTS TOOL_EXES)
      target_include_directories(${t} PRIVATE DirectXTex/Auxiliary)
      target_link_libraries(${t} PRIVATE PNG::PNG)
      target_compile_definitions(${t} PRIVATE USE_LIBPNG)
    endforeach()
  endif()
endif()

if(directxmath_FOUND)
  foreach(t IN LISTS TOOL_EXES)
    target_link_libraries(${t} PRIVATE Microsoft::DirectXMath)
  endforeach()
endif()

if(MSVC)
    foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
      target_compile_options(${t} PRIVATE /Wall /GR-)
    endforeach()
endif()

foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
  target_compile_definitions(${t} PRIVATE ${COMPILER_DEFINES})
  target_compile_options(${t} PRIVATE ${COMPILER_SWITCHES})
  target_link_options(${t} PRIVATE ${LINKER_SWITCHES})
endforeach()

if(MINGW)
    foreach(t IN LISTS TOOL_EXES)
      target_link_options(${t} PRIVATE -municode)
    endforeach()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|IntelLLVM")
    set(WarningsLib -Wall -Wpedantic -Wextra)

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 16.0)
        list(APPEND WarningsLib "-Wno-unsafe-buffer-usage")
    endif()
    target_compile_options(${PROJECT_NAME} PRIVATE ${WarningsLib})

    set(WarningsEXE ${WarningsLib} "-Wno-c++98-compat" "-Wno-c++98-compat-pedantic" "-Wno-switch" "-Wno-switch-enum" "-Wno-switch-default" "-Wno-covered-switch-default" "-Wno-language-extension-token" "-Wno-missing-prototypes" "-Wno-global-constructors" "-Wno-double-promotion")
    foreach(t IN LISTS TOOL_EXES)
      target_compile_options(${t} PRIVATE ${WarningsEXE})
    endforeach()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
      target_compile_options(${t} PRIVATE "-Wno-ignored-attributes" "-Walloc-size-larger-than=4GB")
    endforeach()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 14)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if((CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.37)
       AND (NOT (${DIRECTX_ARCH} MATCHES "^arm"))
       AND ((${DIRECTX_ARCH} MATCHES "x64") OR (CMAKE_SIZEOF_VOID_P EQUAL 8)))
       # Enable since DirectXTex library has a lot of large switch statements
      target_compile_options(${PROJECT_NAME} PRIVATE /jumptablerdata)
    endif()

   if((CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.26)
      AND BUILD_XBOX_EXTS_XBOXONE)
        foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
          target_compile_options(${t} PRIVATE /wd5104 /wd5204)
        endforeach()
    endif()

    set(WarningsEXE "/wd4061" "/wd4062" "/wd4365" "/wd4514" "/wd4625" "/wd4626" "/wd4627" "/wd4668" "/wd4710" "/wd4711" "/wd4751" "/wd4820" "/wd5026" "/wd5027" "/wd5039" "/wd5045" "/wd5219")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.34)
      list(APPEND WarningsEXE "/wd5262" "/wd5264")
    endif()
    foreach(t IN LISTS TOOL_EXES)
      target_compile_options(${t} PRIVATE ${WarningsEXE})
    endforeach()
endif()

if(WIN32)
    if(BUILD_DX12 OR (${DIRECTX_ARCH} MATCHES "^arm64"))
        message(STATUS "Building with DirectX 12 Runtime support")
        set(WINVER 0x0A00)
    elseif(${DIRECTX_ARCH} MATCHES "^arm")
        set(WINVER 0x0602)
    else()
        message(STATUS "Building with Windows 7 compatibility")
        set(WINVER 0x0601)
        target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN7_PLATFORM_UPDATE)
    endif()

    foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
      target_compile_definitions(${t} PRIVATE _WIN32_WINNT=${WINVER})
    endforeach()
endif()

if(BUILD_TOOLS AND BUILD_DX11 AND WIN32)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT texconv)
endif()

if(BUILD_TOOLS AND (NOT VCPKG_TOOLCHAIN))
    foreach(t IN LISTS TOOL_EXES)
      install(TARGETS ${t} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    endforeach()
endif()

#--- Test suite
if(TEXCONV_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
