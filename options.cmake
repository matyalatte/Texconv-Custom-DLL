# hide functions from end users
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# for all options except for exe options and no-gpu option
option(TEXCONV_USE_ALL "Enable 'TEXCONV_USE_*' options" OFF)

# for exe build
option(TEXCONV_BUILD_AS_EXE "Build texconv as .exe" OFF)
option(TEXCONV_ICON_FOR_EXE "Use directx.ico for .exe" OFF)

# other options
option(TEXCONV_USE_WIC "Use WIC for non-DDS formats" ON)
option(TEXCONV_NO_GPU_CODEC "Disable GPU codec for BC6 and BC7" OFF)
option(TEXCONV_USE_TEXASSEMBLE "Use texassemble as a dll function" OFF)

# enable all options
if(TEXCONV_USE_ALL)
  set(TEXCONV_USE_WIC ON)
  set(TEXCONV_NO_GPU_CODEC OFF)
  set(TEXCONV_USE_TEXASSEMBLE ON)
endif()

# disable options for windows
if(NOT WIN32)
  set(TEXCONV_USE_WIC OFF)
  set(TEXCONV_NO_GPU_CODEC ON)
  set(BUILD_DX11 OFF)
  set(BUILD_DX12 OFF)
endif()

if(APPLE)
  # support macOS 11.0 or later
  set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "" FORCE)
  set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
  # Avoid to link to png.h for Mono
  # https://stackoverflow.com/questions/36523911/osx-homebrew-cmake-libpng-version-mismatch-issue
  set(CMAKE_FIND_FRAMEWORK LAST)
endif()

# show variables
function(texconv_show_variables)
  set(ENABLED_OPTIONS "")
  set(PREV_NAME "")
  get_cmake_property(_variableNames VARIABLES)
  foreach (_variableName ${_variableNames})
    if ((_variableName MATCHES "TEXCONV*")
        AND (${_variableName} STREQUAL "ON")
        AND (NOT "${_variableName}" STREQUAL "${PREV_NAME}"))
      set(ENABLED_OPTIONS "${ENABLED_OPTIONS} ${_variableName}\n")
    endif()
    set(PREV_NAME ${_variableName})
  endforeach()
  if (ENABLED_OPTIONS STREQUAL "")
    message(STATUS "No enabled options for texconv.")
  else()
    message(STATUS "Enabled texconv options")
    message(${ENABLED_OPTIONS})
  endif()
endfunction()
texconv_show_variables()

set(BUILD_SAMPLE OFF)
