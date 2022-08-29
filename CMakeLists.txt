# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.13)

set(DIRECTXTEX_VERSION 1.9.7)

# for exe build
option(TEXCONV_BUILD_AS_EXE "Build texconv as .exe" OFF)
option(TEXCONV_USE_ICON_FOR_EXE "Use directx.ico for .exe" OFF)

# for texture formats
option(TEXCONV_USE_WIC "Use WIC-supported format (bmp, jpg, png, jxr, etc.)" OFF)
option(TEXCONV_USE_PPM "Use .ppm and .pfm" OFF)
option(TEXCONV_USE_PLANAR "Use planar dds formats" OFF)
option(TEXCONV_USE_3D "Use 3D textures" OFF)
option(TEXCONV_USE_LEGACY "Use legacy dds formats" OFF)
option(TEXCONV_USE_16BPP "Use 16bpp format (B5G6R5, B5G5R5A1, B4G4R4A4)" OFF)

# for printing
option(TEXCONV_USE_LOGO "Use PrintLogo()" OFF)
option(TEXCONV_USE_USAGE "Use PrintUsage()" OFF)
option(TEXCONV_USE_PRINT_INFO "Use PrintInfo() and PrintFormat()" OFF)
option(TEXCONV_USE_TIMING "Use -timing" OFF)

# other options
option(TEXCONV_USE_FLIP "Use -hflip and -vflip" OFF)
option(TEXCONV_USE_SWIZZLE "Use -swizzle" OFF)
option(TEXCONV_USE_ROTATE_COLOR "Use -rotatecolor" OFF)
option(TEXCONV_USE_COLORKEY "Use -c" OFF)
option(TEXCONV_USE_ALPHA_CONFIG "Use -pmalpha, -alpha, -keepcoverage, -aw, and -at" OFF)
option(TEXCONV_USE_ADDRESSING "Use -wrap and -mirror" OFF)
option(TEXCONV_USE_TONEMAP "Use -tonemap" OFF)
option(TEXCONV_USE_MULTIPLE_FILES "Use -r and -flist" OFF)
option(TEXCONV_USE_NMAP_CONFIG "Use -nmap and -nmapamp" OFF)
option(TEXCONV_USE_MINOR_DDS_CONFIG "Use -tu, -tf, -dword, -badtails, -fixbc4x4, -xlum, -dx10, and -dx9" OFF)
option(TEXCONV_USE_FEATURE_LEVEL "Use -fl" OFF)
option(TEXCONV_USE_BC_CONFIG "Use -bc" OFF)
option(TEXCONV_USE_SRGB "Use -srgb, -srgbi, and -srgbo" OFF)
option(TEXCONV_USE_NAME_CONFIG "Use -px, -sx, and -l" OFF)
option(TEXCONV_USE_DITHER "Use dither filters" OFF)
option(TEXCONV_USE_GPU_CONFIG "Use -gpu and -nogpu" OFF)
option(TEXCONV_USE_X2BIAS "Use -x2bias" OFF)
option(TEXCONV_USE_SINGLEPROC "Use -singleproc" OFF)
option(TEXCONV_USE_TGA20 "Use -tga20 " OFF)

project (DirectXTex
  VERSION ${DIRECTXTEX_VERSION}
  DESCRIPTION "DirectX Texture Library"
  HOMEPAGE_URL "https://go.microsoft.com/fwlink/?LinkId=248926"
  LANGUAGES CXX)

option(BUILD_TOOLS "Build tex command-line tools" ON)

option(BUILD_SAMPLE "Build DDSView sample" OFF)

# Includes the functions for Direct3D 11 resources and DirectCompute compression
option(BUILD_DX11 "Build with DirectX11 Runtime support" ON)

# Includes the functions for creating Direct3D 12 resources at runtime
option(BUILD_DX12 "Build with DirectX12 Runtime support" ON)

# Enable the use of OpenMP for software BC6H/BC7 compression
option(BC_USE_OPENMP "Build with OpenMP support" ON)

option(ENABLE_CODE_ANALYSIS "Use Static Code Analysis on build" OFF)

option(USE_PREBUILT_SHADERS "Use externally built HLSL shaders" OFF)

# Includes the functions for loading/saving OpenEXR files at runtime
# NOTE requires adding DirectXTexEXR.h/.cpp source files (vcpkg does this automatically)
option(ENABLE_OPENEXR_SUPPORT "Build with OpenEXR support" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/CMake")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/CMake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/CMake")

if (DEFINED VCPKG_TARGET_ARCHITECTURE)
    set(DIRECTX_ARCH ${VCPKG_TARGET_ARCHITECTURE})
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Ww][Ii][Nn]32$")
    set(DIRECTX_ARCH x86)
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Xx]64$")
    set(DIRECTX_ARCH x64)
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Aa][Rr][Mm]$")
    set(DIRECTX_ARCH arm)
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Aa][Rr][Mm]64$")
    set(DIRECTX_ARCH arm64)
endif()

include(GNUInstallDirs)

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
    TexconvDLL/DirectXTexConvert_Custom.cpp
    TexconvDLL/DirectXTexDDS_Custom.cpp
    DirectXTex/DirectXTex/DirectXTexHDR.cpp
    TexconvDLL/DirectXTexImage_Custom.cpp
    TexconvDLL/DirectXTexMipmaps_Custom.cpp
    TexconvDLL/DirectXTexMisc_Custom.cpp
    DirectXTex/DirectXTex/DirectXTexNormalMaps.cpp
    TexconvDLL/DirectXTexResize_Custom.cpp
    DirectXTex/DirectXTex/DirectXTexTGA.cpp
    DirectXTex/DirectXTex/DirectXTexUtil.cpp)

if(TEXCONV_USE_ALPHA_CONFIG)
  set(LIBRARY_SOURCES ${LIBRARY_SOURCES}
    DirectXTex/DirectXTex/DirectXTexPMAlpha.cpp)
endif()

if(WIN32)
  if(TEXCONV_USE_FLIP OR TEXCONV_USE_WIC)
    set(LIBRARY_SOURCES ${LIBRARY_SOURCES}
        DirectXTex/DirectXTex/DirectXTexFlipRotate.cpp
        DirectXTex/DirectXTex/DirectXTexWIC.cpp)
  endif()
endif()

set(SHADER_SOURCES
    DirectXTex/DirectXTex/Shaders/BC6HEncode.hlsl
    DirectXTex/DirectXTex/Shaders/BC7Encode.hlsl)

if(BUILD_DX11 AND WIN32)
   set(LIBRARY_SOURCES ${LIBRARY_SOURCES}
       DirectXTex/DirectXTex/BCDirectCompute.h
       DirectXTex/DirectXTex/BCDirectCompute.cpp
       TexconvDLL/DirectXTexCompressGPU_Custom.cpp
       DirectXTex/DirectXTex/DirectXTexD3D11.cpp)
endif()

if(BUILD_DX12)
   set(LIBRARY_SOURCES ${LIBRARY_SOURCES}
       DirectXTex/DirectXTex/d3dx12.h
       DirectXTex/DirectXTex/DirectXTexD3D12.cpp)
endif()

if(ENABLE_OPENEXR_SUPPORT)
   set(LIBRARY_HEADERS ${LIBRARY_HEADERS}
       DirectXTex/DirectXTex/DirectXTexEXR.h)

   set(LIBRARY_SOURCES ${LIBRARY_SOURCES}
       DirectXTex/DirectXTex/DirectXTexEXR.h
       DirectXTex/DirectXTex/DirectXTexEXR.cpp)
endif()

if(BUILD_DX11 AND WIN32)
    if (NOT COMPILED_SHADERS)
        if (USE_PREBUILT_SHADERS)
            message(FATAL_ERROR "ERROR: Using prebuilt shaders requires the COMPILED_SHADERS variable is set")
        endif()
        set(COMPILED_SHADERS ${CMAKE_CURRENT_BINARY_DIR}/DirectXTex/Shaders/Compiled)
        file(MAKE_DIRECTORY ${COMPILED_SHADERS})
    else()
        file(TO_CMAKE_PATH ${COMPILED_SHADERS} COMPILED_SHADERS)
    endif()

    set(LIBRARY_SOURCES ${LIBRARY_SOURCES}
        ${COMPILED_SHADERS}/BC6HEncode_EncodeBlockCS.inc)

    if (NOT USE_PREBUILT_SHADERS)
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
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DirectXTex/DirectXTex>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

if(ENABLE_OPENEXR_SUPPORT)
  find_package(OpenEXR REQUIRED)
  target_include_directories(${PROJECT_NAME} PRIVATE ${OPENEXR_INCLUDE_DIRS}/OpenEXR)
endif()

if ((${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16") AND (NOT MINGW))
    target_precompile_headers(${PROJECT_NAME} PRIVATE DirectXTex/DirectXTex/DirectXTexP.h)
endif()

if(MSVC)
    # Use max Warning Level
    string(REPLACE "/W3 " "/Wall " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    string(REPLACE "/W3 " "/Wall " CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    string(REPLACE "/W3 " "/Wall " CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

    # Not using typeid or dynamic_cast, so disable RTTI to save binary size
    string(REPLACE "/GR " "/GR- " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    string(REPLACE "/GR " "/GR- " CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    string(REPLACE "/GR " "/GR- " CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
endif()

if (MINGW OR (NOT WIN32) OR VCPKG_TOOLCHAIN)
    message("INFO: Using VCPKG for DirectX-Headers and DirectXMath.")
    find_package(directx-headers CONFIG REQUIRED)
    find_package(directxmath CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectX-Headers Microsoft::DirectXMath)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USING_DIRECTX_HEADERS)
endif()

#--- Package
include(CMakePackageConfigHelpers)

string(TOLOWER ${PROJECT_NAME} PACKAGE_NAME)

write_basic_package_version_file(
  ${PACKAGE_NAME}-config-version.cmake
  VERSION ${DIRECTXTEX_VERSION}
  COMPATIBILITY AnyNewerVersion)

install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/DirectXTex/.nuget/${PROJECT_NAME}-config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PACKAGE_NAME})

install(EXPORT ${PROJECT_NAME}-targets
  FILE ${PROJECT_NAME}-targets.cmake
  NAMESPACE Microsoft::
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PACKAGE_NAME})

install(FILES ${LIBRARY_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-config-version.cmake
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PACKAGE_NAME})

#--- Command-line tools
if(BUILD_TOOLS AND WIN32 AND (NOT WINDOWS_STORE))
  set(TOOL_EXES texconv)

  configure_file(TexconvDLL/config.h.in TexconvDLL/config.h @ONLY)
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  set(TEXCONV_FILES
    TexconvDLL/config.h
    TexconvDLL/TexconvDLL.cpp)
  if (TEXCONV_USE_WIC)
    set(TEXCONV_FILES ${TEXCONV_FILES}
      DirectXTex/Texconv/ExtendedBMP.cpp)
  endif()
  if (TEXCONV_USE_PPM)
    set(TEXCONV_FILES ${TEXCONV_FILES}
      DirectXTex/Texconv/PortablePixMap.cpp)
  endif()
  if (TEXCONV_BUILD_AS_EXE)
    if (TEXCONV_USE_ICON_FOR_EXE)
      set(TEXCONV_FILES ${TEXCONV_FILES}
        TexconvDLL/TexconvDLL.rc)
    endif()
    add_executable(texconv ${TEXCONV_FILES})
  else()
    add_library(texconv SHARED ${TEXCONV_FILES})
  endif()
  source_group(texconv FILES ${TEXCONV_FILES})
  target_link_libraries(texconv ${PROJECT_NAME})
  if(WIN32)
    target_link_libraries(texconv ole32.lib shell32.lib version.lib)
  endif()

  if(ENABLE_OPENEXR_SUPPORT)
    foreach(t IN LISTS TOOL_EXES)
      target_link_libraries(${t} ${OPENEXR_ILMIMF_LIBRARY})
      target_compile_options(${t} PRIVATE -DUSE_OPENEXR)
    endforeach()
  endif()
endif()

if (MINGW OR (NOT WIN32) OR VCPKG_TOOLCHAIN)
  foreach(t IN LISTS TOOL_EXES)
    target_link_libraries(${t} Microsoft::DirectXMath)
  endforeach()
endif()

if(MSVC)
    foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
      target_compile_options(${t} PRIVATE /fp:fast "$<$<NOT:$<CONFIG:DEBUG>>:/guard:cf>")
      target_link_options(${t} PRIVATE /DYNAMICBASE /NXCOMPAT)
    endforeach()

    if((${CMAKE_SIZEOF_VOID_P} EQUAL 4) AND (NOT ${DIRECTX_ARCH} MATCHES "^arm"))
        foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
          target_link_options(${t} PRIVATE /SAFESEH)
        endforeach()
    endif()

    if((MSVC_VERSION GREATER_EQUAL 1928) AND (CMAKE_SIZEOF_VOID_P EQUAL 8)
       AND ((NOT (CMAKE_CXX_COMPILER_ID MATCHES "Clang")) OR (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 13.0)))
      foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
        target_compile_options(${t} PRIVATE "$<$<NOT:$<CONFIG:DEBUG>>:/guard:ehcont>")
        target_link_options(${t} PRIVATE "$<$<NOT:$<CONFIG:DEBUG>>:/guard:ehcont>")
      endforeach()
    endif()
endif()

if(NOT ${DIRECTX_ARCH} MATCHES "^arm")
    if (${CMAKE_SIZEOF_VOID_P} EQUAL "4")
        set(ARCH_SSE2 $<$<CXX_COMPILER_ID:MSVC>:/arch:SSE2> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-msse2>)
    else()
        set(ARCH_SSE2 $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-msse2>)
    endif()

    foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
      target_compile_options(${t} PRIVATE ${ARCH_SSE2})
    endforeach()
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(WarningsLib "-Wpedantic" "-Wextra")
    target_compile_options(${PROJECT_NAME} PRIVATE ${WarningsLib})

    # OpenMP is not supported for clang for Windows by default

    set(WarningsEXE ${WarningsLib} "-Wno-c++98-compat" "-Wno-c++98-compat-pedantic" "-Wno-switch" "-Wno-switch-enum" "-Wno-covered-switch-default" "-Wno-language-extension-token" "-Wno-missing-prototypes" "-Wno-global-constructors" "-Wno-double-promotion")
    foreach(t IN LISTS TOOL_EXES)
      target_compile_options(${t} PRIVATE ${WarningsEXE})
    endforeach()
endif()
if(MINGW)
    foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
      target_compile_options(${t} PRIVATE -Wno-ignored-attributes)
      target_link_options(${t} PRIVATE -municode)
    endforeach()
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
      target_compile_options(${t} PRIVATE /sdl /permissive- /JMC- /Zc:__cplusplus)
    endforeach()

    if(ENABLE_CODE_ANALYSIS)
      foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
        target_compile_options(${t} PRIVATE /analyze)
      endforeach()
    endif()

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.24)
        foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
          target_compile_options(${t} PRIVATE /ZH:SHA_256)
        endforeach()
    endif()

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.26)
        foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
          target_compile_options(${t} PRIVATE /Zc:preprocessor /wd5105)
        endforeach()
    endif()

    if ((CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.27) AND (NOT (${DIRECTX_ARCH} MATCHES "^arm")))
        foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
          target_link_options(${t} PRIVATE /CETCOMPAT)
        endforeach()
    endif()

    if(BC_USE_OPENMP)
        target_compile_options(${PROJECT_NAME} PRIVATE /openmp /Zc:twoPhase-)
        if(BUILD_TOOLS AND WIN32 AND (NOT WINDOWS_STORE))
          target_compile_options(texconv PRIVATE /openmp /Zc:twoPhase-)
        endif()
    endif()

    set(WarningsEXE "/wd4061" "/wd4062" "/wd4365" "/wd4514" "/wd4625" "/wd4626" "/wd4627" "/wd4668" "/wd4710" "/wd4751" "/wd4820" "/wd5026" "/wd5027" "/wd5039" "/wd5045" "/wd5219")
    foreach(t IN LISTS TOOL_EXES)
      target_compile_options(${t} PRIVATE ${WarningsEXE})
    endforeach()
endif()

if(WIN32)
    foreach(t IN LISTS TOOL_EXES ITEMS ${PROJECT_NAME})
      target_compile_definitions(${t} PRIVATE _UNICODE UNICODE)
    endforeach()

    if(WINDOWS_STORE)
      target_compile_definitions(${PROJECT_NAME} PRIVATE WINAPI_FAMILY=WINAPI_FAMILY_APP)
    endif()

    if(BUILD_DX12 OR WINDOWS_STORE)
        if(BUILD_DX12)
          message("INFO: Building with DirectX 12 Runtime support")
        endif()
        target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0A00)
    else()
        message("INFO: Building with Windows 7 compatibility")
        target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0601 _WIN7_PLATFORM_UPDATE)
    endif()
    foreach(t IN LISTS TOOL_EXES)
      target_compile_definitions(${t} PRIVATE _WIN32_WINNT=0x0601)
    endforeach()
endif()

if(BUILD_TOOLS AND WIN32 AND (NOT WINDOWS_STORE))
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Texconv)
endif()
