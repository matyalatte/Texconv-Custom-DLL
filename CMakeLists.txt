# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.20)

set(DIRECTXTEX_VERSION 2.0.4)

if(WINDOWS_STORE OR (DEFINED XBOX_CONSOLE_TARGET))
  set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
endif()

project (DirectXTex
  VERSION ${DIRECTXTEX_VERSION}
  DESCRIPTION "DirectX Texture Library"
  HOMEPAGE_URL "https://go.microsoft.com/fwlink/?LinkId=248926"
  LANGUAGES C CXX)

option(BUILD_TOOLS "Build tex command-line tools" ON)

option(BUILD_SAMPLE "Build DDSView sample (requires fxc.exe)" ON)

# Includes the functions for Direct3D 11 resources and DirectCompute compression
option(BUILD_DX11 "Build with DirectX11 Runtime support (requires fxc.exe)" ON)

# Includes the functions for creating Direct3D 12 resources at runtime
option(BUILD_DX12 "Build with DirectX12 Runtime support" ON)

# Enable the use of OpenMP for software BC6H/BC7 compression
option(BC_USE_OPENMP "Build with OpenMP support" ON)

# Builds Xbox extensions for Host PC
option(BUILD_XBOX_EXTS_XBOXONE "Build Xbox library extensions for Xbox One" OFF)
option(BUILD_XBOX_EXTS_SCARLETT "Build Xbox library extensions for Xbox Series X|S" OFF)

# https://devblogs.microsoft.com/cppblog/spectre-mitigations-in-msvc/
option(ENABLE_SPECTRE_MITIGATION "Build using /Qspectre for MSVC" OFF)

option(DISABLE_MSVC_ITERATOR_DEBUGGING "Disable iterator debugging in Debug configurations with the MSVC CRT" OFF)

option(ENABLE_CODE_ANALYSIS "Use Static Code Analysis on build" OFF)

option(USE_PREBUILT_SHADERS "Use externally built HLSL shaders" OFF)

option(NO_WCHAR_T "Use legacy wide-character as unsigned short" OFF)

option(BUILD_FUZZING "Build for fuzz testing" OFF)

# Includes the functions for loading/saving OpenEXR files at runtime
option(ENABLE_OPENEXR_SUPPORT "Build with OpenEXR support" OFF)

# See https://www.ijg.org/
option(ENABLE_LIBJPEG_SUPPORT "Build with libjpeg support" OFF)

# See http://www.libpng.org/pub/png/libpng.html
option(ENABLE_LIBPNG_SUPPORT "Build with libpng support" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(WINDOWS_STORE OR (DEFINED XBOX_CONSOLE_TARGET))
   set(BUILD_DX12 ON)
   set(BUILD_TOOLS OFF)
   set(BUILD_SAMPLE OFF)
endif()

if((DEFINED XBOX_CONSOLE_TARGET) AND (NOT (XBOX_CONSOLE_TARGET STREQUAL "durango")))
  set(BUILD_DX11 OFF)
endif()

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

if(DEFINED XBOX_CONSOLE_TARGET)

    list(APPEND LIBRARY_HEADERS Auxiliary/DirectXTexXbox.h)

    list(APPEND LIBRARY_SOURCES
       Auxiliary/DirectXTexXboxDDS.cpp
       Auxiliary/DirectXTexXboxDetile.cpp
       Auxiliary/DirectXTexXboxImage.cpp
       Auxiliary/DirectXTexXboxTile.cpp)

    if((XBOX_CONSOLE_TARGET STREQUAL "durango") AND BUILD_DX11)
       list(APPEND LIBRARY_SOURCES Auxiliary/DirectXTexXboxD3D11X.cpp)
    endif()

    if(BUILD_DX12)
       list(APPEND LIBRARY_SOURCES Auxiliary/DirectXTexXboxD3D12X.cpp)
    endif()

elseif((BUILD_XBOX_EXTS_XBOXONE OR BUILD_XBOX_EXTS_SCARLETT) AND WIN32)
    if(DEFINED ENV{GameDKLatest})
        cmake_path(SET GameDK_DIR "$ENV{GameDKLatest}")
    endif()
    if(DEFINED ENV{XboxOneXDKLatest})
        cmake_path(SET XboxOneXDK_DIR "$ENV{XboxOneXDKLatest}")
    endif()

    list(APPEND LIBRARY_HEADERS Auxiliary/DirectXTexXbox.h)

    list(APPEND LIBRARY_SOURCES
       Auxiliary/DirectXTexXboxDDS.cpp
       Auxiliary/DirectXTexXboxDetile.cpp
       Auxiliary/DirectXTexXboxImage.cpp
       Auxiliary/DirectXTexXboxTile.cpp)
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
       list(APPEND LIBRARY_SOURCES DirectXTex/DirectXTex/d3dx12.h)
   endif()
endif()

if(ENABLE_OPENEXR_SUPPORT)
   list(APPEND LIBRARY_HEADERS Auxiliary/DirectXTexEXR.h)
   list(APPEND LIBRARY_SOURCES Auxiliary/DirectXTexEXR.cpp)
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
        add_custom_command(
            OUTPUT "${COMPILED_SHADERS}/BC6HEncode_EncodeBlockCS.inc"
            MAIN_DEPENDENCY "${PROJECT_SOURCE_DIR}/DirectXTex/DirectXTex/Shaders/CompileShaders.cmd"
            DEPENDS ${SHADER_SOURCES}
            COMMENT "Generating HLSL shaders..."
            COMMAND ${CMAKE_COMMAND} -E env CompileShadersOutput="${COMPILED_SHADERS}" CompileShaders.cmd > "${COMPILED_SHADERS}/compileshaders.log"
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
  find_package(OpenEXR REQUIRED)
  target_link_libraries(${PROJECT_NAME} PUBLIC OpenEXR::OpenEXR)
endif()

if(BUILD_XBOX_EXTS_SCARLETT AND WIN32)
    if(EXISTS "${GameDK_DIR}/GXDK/toolKit/include/gxdk.h")
        message(STATUS "Building Xbox extensions for Xbox Series X|S")

        add_library(Xbox::GDKX SHARED IMPORTED)
        set_target_properties(Xbox::GDKX PROPERTIES
            IMPORTED_LOCATION "${GameDK_DIR}/GXDK/bin/Scarlett/xg_xs.dll"
            IMPORTED_IMPLIB "${GameDK_DIR}/GXDK/toolKit/lib/amd64/Scarlett/xg_xs.lib"
            MAP_IMPORTED_CONFIG_MINSIZEREL ""
            MAP_IMPORTED_CONFIG_RELWITHDEBINFO ""
            INTERFACE_COMPILE_DEFINITIONS "_USE_GXDK;_USE_SCARLETT"
            INTERFACE_INCLUDE_DIRECTORIES "${GameDK_DIR}/GXDK/toolKit/include;${GameDK_DIR}/GXDK/toolKit/include/Scarlett")

        target_compile_definitions(${PROJECT_NAME} PRIVATE USE_XBOX_EXTS)
        target_link_libraries(${PROJECT_NAME} PUBLIC Xbox::GDKX)
    else()
        message(FATAL_ERROR "Building Xbox extensions requires GameDKLatest")
    endif()
elseif(BUILD_XBOX_EXTS_XBOXONE AND WIN32)
    if(EXISTS "${GameDK_DIR}/GXDK/toolKit/include/gxdk.h")
        message(STATUS "Building Xbox extensions for XboxOne using the Microsoft GDK")

        add_library(Xbox::GDKX SHARED IMPORTED)
        set_target_properties(Xbox::GDKX PROPERTIES
            IMPORTED_LOCATION "${GameDK_DIR}/GXDK/bin/XboxOne/xg.dll"
            IMPORTED_IMPLIB "${GameDK_DIR}/GXDK/toolKit/lib/amd64/XboxOne/xg.lib"
            MAP_IMPORTED_CONFIG_MINSIZEREL ""
            MAP_IMPORTED_CONFIG_RELWITHDEBINFO ""
            INTERFACE_COMPILE_DEFINITIONS "_USE_GXDK"
            INTERFACE_INCLUDE_DIRECTORIES "${GameDK_DIR}/GXDK/toolKit/include;${GameDK_DIR}/GXDK/toolKit/include/XboxOne")

        target_compile_definitions(${PROJECT_NAME} PRIVATE USE_XBOX_EXTS)
        target_link_libraries(${PROJECT_NAME} PUBLIC Xbox::GDKX)

    elseif(EXISTS "${XboxOneXDK_DIR}/PC/include/xdk.h")
        message(STATUS "Building Xbox extensions for XboxOne using the Xbox One XDK")

        add_library(Xbox::XDK SHARED IMPORTED)
        set_target_properties(Xbox::XDK PROPERTIES
            IMPORTED_LOCATION "${XboxOneXDK_DIR}/bin/xg.dll"
            IMPORTED_IMPLIB "${XboxOneXDK_DIR}/PC/lib/amd64/xg.lib"
            MAP_IMPORTED_CONFIG_MINSIZEREL ""
            MAP_IMPORTED_CONFIG_RELWITHDEBINFO ""
            INTERFACE_INCLUDE_DIRECTORIES "${XboxOneXDK_DIR}/PC/include")

        target_compile_definitions(${PROJECT_NAME} PRIVATE USE_XBOX_EXTS)
        target_link_libraries(${PROJECT_NAME} PUBLIC Xbox::XDK)
    else()
        message(FATAL_ERROR "Building Xbox extensions requires GameDKLatest or XboxOneXDKLatest")
    endif()
endif()

if(ENABLE_LIBJPEG_SUPPORT)
  find_package(JPEG REQUIRED)
  target_link_libraries(${PROJECT_NAME} PUBLIC JPEG::JPEG)
endif()

if(ENABLE_LIBPNG_SUPPORT)
  find_package(PNG REQUIRED)
  target_link_libraries(${PROJECT_NAME} PUBLIC PNG::PNG)
endif()

if(NOT MINGW)
    target_precompile_headers(${PROJECT_NAME} PRIVATE DirectXTex/DirectXTex/DirectXTexP.h)
endif()

if(MINGW OR (NOT WIN32))
    if (WIN32)
      find_package(directxmath CONFIG REQUIRED)
      find_package(directx-headers CONFIG REQUIRED)
    else()
      message("INFO: Using a subdirectory 'unix_external' for DirectX-Headers, DirectXMath, and safestringlib.")
      add_subdirectory(unix_external)
    endif()
else()
    find_package(directxmath CONFIG QUIET)
    find_package(directx-headers CONFIG QUIET)
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
      DirectXTex/Texassemble/settings.manifest)
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
        DirectXTex/Texconv/settings.manifest)
    endif()
    add_executable(texconv ${TEXCONV_FILES})
  else()
    add_library(texconv SHARED ${TEXCONV_FILES})
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

if(FALSE)
  add_executable(texdiag
    Texdiag/texdiag.cpp
    Texdiag/texdiag.rc
    Texdiag/settings.manifest)
  target_compile_features(texdiag PRIVATE cxx_std_17)
  target_link_libraries(texdiag PRIVATE ${PROJECT_NAME} ole32.lib version.lib)
  source_group(texdiag REGULAR_EXPRESSION Texdiag/*.*)
  list(APPEND TOOL_EXES texdiag)
endif()

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
  if(BUILD_XBOX_EXTS_SCARLETT OR BUILD_XBOX_EXTS_XBOXONE)
    target_include_directories(texconv PRIVATE DirectXTex/Auxiliary)
    target_compile_definitions(texconv PRIVATE USE_XBOX_EXTS)
    target_link_libraries(texconv PUBLIC $<TARGET_NAME_IF_EXISTS:Xbox::GDKX> $<TARGET_NAME_IF_EXISTS:Xbox::XDK>)

    if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.21")
      add_custom_command(TARGET texconv POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy
          $<TARGET_RUNTIME_DLLS:texconv> $<TARGET_FILE_DIR:texconv>
          COMMAND_EXPAND_LISTS
          )
    endif()
  endif()
endif()

#--- DDSView sample
if(BUILD_SAMPLE AND BUILD_DX11 AND WIN32)
  list(APPEND TOOL_EXES ddsview)

  if(NOT COMPILED_DDSVIEW_SHADERS)
      if(USE_PREBUILT_SHADERS)
          message(FATAL_ERROR "ERROR: Using prebuilt shaders requires the COMPILED_DDSVIEW_SHADERS variable is set")
      endif()
      set(COMPILED_DDSVIEW_SHADERS ${CMAKE_CURRENT_BINARY_DIR}/Shaders/Compiled)
      file(MAKE_DIRECTORY ${COMPILED_DDSVIEW_SHADERS})
  else()
      file(TO_CMAKE_PATH ${COMPILED_DDSVIEW_SHADERS} COMPILED_DDSVIEW_SHADERS)
  endif()

  add_executable(ddsview WIN32
    DDSView/ddsview.cpp
    DDSView/ddsview.rc
    ${COMPILED_DDSVIEW_SHADERS}/ddsview_ps1D.inc)
  target_link_libraries(ddsview PRIVATE ${PROJECT_NAME} d3d11.lib ole32.lib)
  source_group(ddsview REGULAR_EXPRESSION DDSView/*.*)

  target_include_directories(ddsview PRIVATE ${COMPILED_DDSVIEW_SHADERS})

  if(NOT USE_PREBUILT_SHADERS)
      add_custom_command(
            OUTPUT "${COMPILED_DDSVIEW_SHADERS}/ddsview_ps1D.inc"
            MAIN_DEPENDENCY "${PROJECT_SOURCE_DIR}/DDSView/hlsl.cmd"
            DEPENDS "${PROJECT_SOURCE_DIR}/DDSView/ddsview.fx"
            COMMENT "Generating HLSL shaders for DDSView..."
            COMMAND COMMAND ${CMAKE_COMMAND} -E env CompileShadersOutput="${COMPILED_DDSVIEW_SHADERS}" hlsl.cmd > "${COMPILED_DDSVIEW_SHADERS}/hlsl.log"
            WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/DDSView"
            USES_TERMINAL)
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

    if(NO_WCHAR_T)
      message(STATUS "Using non-native wchar_t as unsigned short")
      foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
        target_compile_options(${t} PRIVATE "/Zc:wchar_t-")
      endforeach()
    endif()
endif()

include(DirectXTex/build/CompilerAndLinker.cmake)

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

    if((BUILD_XBOX_EXTS_XBOXONE OR BUILD_XBOX_EXTS_SCARLETT) AND WIN32)
        list(APPEND WarningsLib "-Wno-microsoft-enum-value" "-Wno-non-virtual-dtor" "-Wno-ignored-pragmas" "-Wno-deprecated-dynamic-exception-spec")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 13.0)
            list(APPEND WarningsLib "-Wno-reserved-identifier")
        endif()
    endif()

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

    if(ENABLE_CODE_ANALYSIS)
      foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
        target_compile_options(${t} PRIVATE /analyze)
      endforeach()
    endif()

    if(ENABLE_SPECTRE_MITIGATION
       AND (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.13)
       AND (NOT ENABLE_OPENEXR_SUPPORT)
       AND (NOT WINDOWS_STORE))
        message(STATUS "Building Spectre-mitigated libraries")
        foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
          target_compile_options(${t} PRIVATE "/Qspectre")
        endforeach()
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

    if(BUILD_FUZZING
       AND (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.32)
       AND (NOT WINDOWS_STORE))
          foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
            target_compile_options(${t} PRIVATE ${ASAN_SWITCHES})
            target_link_libraries(${t} PRIVATE ${ASAN_LIBS})
          endforeach()
    endif()
endif()

if(WIN32)
    if(XBOX_CONSOLE_TARGET STREQUAL "durango")
        set(WINVER 0x0602)
    elseif(BUILD_DX12 OR (${DIRECTX_ARCH} MATCHES "^arm64"))
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

    if(DISABLE_MSVC_ITERATOR_DEBUGGING)
      foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
        target_compile_definitions(${t} PRIVATE _ITERATOR_DEBUG_LEVEL=0)
      endforeach()
    endif()
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
if(WIN32 AND (NOT WINDOWS_STORE) AND (NOT (DEFINED XBOX_CONSOLE_TARGET)))
    include(CTest)
    if(BUILD_TESTING AND (EXISTS "${CMAKE_CURRENT_LIST_DIR}/Tests/CMakeLists.txt"))
        enable_testing()
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Tests)
    elseif(BUILD_FUZZING AND (EXISTS "${CMAKE_CURRENT_LIST_DIR}/Tests/fuzzloaders/CMakeLists.txt"))
        message(STATUS "Building for fuzzing")
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Tests/fuzzloaders)
    endif()
endif()
