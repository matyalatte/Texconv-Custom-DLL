# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.20)

set(DIRECTXTEX_VERSION 2.0.9)

project (Texconv-Custom-DLL
  LANGUAGES C CXX)

set(BUILD_TOOLS OFF)
set(BUILD_SAMPLE OFF)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CMAKE_SKIP_INSTALL_RULES ON CACHE BOOL "" FORCE)

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
include("cmake/options.cmake")

#--- Third Party Libraries
if(TEXCONV_USE_STATIC_LINK)
    # We build third party libraries from source.
    # So, let find_package() open fake .cmake files
    list(PREPEND CMAKE_MODULE_PATH
        "${CMAKE_SOURCE_DIR}/cmake/override")
endif()

if(ENABLE_OPENEXR_SUPPORT)
  if(TEXCONV_USE_STATIC_LINK)
    include(FetchContent)
    set(OPENEXR_INSTALL OFF)
    set(OPENEXR_BUILD_TOOLS OFF)
    set(OPENEXR_BUILD_TOOLS OFF)
    set(OPENEXR_BUILD_EXAMPLES OFF)
    set(OPENEXR_FORCE_INTERNAL_DEFLATE ON)
    set(OPENEXR_FORCE_INTERNAL_OPENJPH ON)
    set(OPENEXR_FORCE_INTERNAL_IMATH ON)
    # Remove symbols from OpenEXR
    set(OPENEXR_USE_DEFAULT_VISIBILITY ON)
    # Remove symbols from Imath
    set(IMATH_USE_DEFAULT_VISIBILITY ON)
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
endif()

if(WIN32 AND NOT MINGW)
    find_package(directxmath CONFIG QUIET)
    find_package(directx-headers CONFIG QUIET)
else()
    message("INFO: Using a subdirectory 'unix_external' for DirectX-Headers, DirectXMath, and safestringlib.")
    add_subdirectory(unix_external)
    set(directx-headers_FOUND ON)
    set(directxmath_FOUND ON)
    # We should override find_package() for directx-headers and directxmath
    list(PREPEND CMAKE_PREFIX_PATH
        "${CMAKE_SOURCE_DIR}/cmake/override")
endif()

# Build DirectXTex
add_subdirectory(DirectXTex)

# Reset overwritten variables
set(BUILD_TOOLS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include(DirectXTex/build/CompilerAndLinker.cmake)

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
    target_link_libraries(texassemble PRIVATE DirectXTex ole32.lib version.lib)
  else()
    # unix need a library for safe functions
    target_link_libraries(texassemble PRIVATE DirectXTex safestring_static)
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
    target_link_libraries(texconv PRIVATE DirectXTex ole32.lib shell32.lib version.lib)
  else()
    # unix need a library for safe functions
    target_link_libraries(texconv PRIVATE DirectXTex safestring_static)
  endif()
  source_group(texconv FILES ${TEXCONV_FILES})
  list(APPEND TOOL_EXES texconv)
endif()

foreach(t IN LISTS TOOL_EXES ITEMS)
  target_include_directories(${t} PRIVATE DirectXTex/Common ${CMAKE_BINARY_DIR}/TexconvDLL)
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
    foreach(t IN LISTS TOOL_EXES ITEMS)
      target_compile_options(${t} PRIVATE /Wall /GR-)
    endforeach()
endif()

foreach(t IN LISTS TOOL_EXES ITEMS)
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

    set(WarningsEXE ${WarningsLib} "-Wno-c++98-compat" "-Wno-c++98-compat-pedantic" "-Wno-switch" "-Wno-switch-enum" "-Wno-switch-default" "-Wno-covered-switch-default" "-Wno-language-extension-token" "-Wno-missing-prototypes" "-Wno-global-constructors" "-Wno-double-promotion")
    foreach(t IN LISTS TOOL_EXES)
      target_compile_options(${t} PRIVATE ${WarningsEXE})
    endforeach()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    foreach(t IN LISTS TOOL_EXES ITEMS)
      target_compile_options(${t} PRIVATE "-Wno-ignored-attributes" "-Walloc-size-larger-than=4GB")
    endforeach()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
   if((CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.26)
      AND BUILD_XBOX_EXTS_XBOXONE)
        foreach(t IN LISTS TOOL_EXES ITEMS)
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
    endif()

    foreach(t IN LISTS TOOL_EXES ITEMS DirectXTex)
      target_compile_definitions(${t} PRIVATE _WIN32_WINNT=${WINVER})
    endforeach()
endif()

if(BUILD_TOOLS AND BUILD_DX11 AND WIN32)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT texconv)
endif()

#--- Test suite
if(TEXCONV_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
