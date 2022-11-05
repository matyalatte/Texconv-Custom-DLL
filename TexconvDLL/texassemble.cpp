//--------------------------------------------------------------------------------------
// File: Texassemble.cpp
//
// DirectX Texture assembler for cube maps, volume maps, and arrays
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248926
//--------------------------------------------------------------------------------------

#include "TexconvDLL/config.h"

#pragma warning(push)
#pragma warning(disable : 4005)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NODRAWTEXT
#define NOGDI
#define NOMCX
#define NOSERVICE
#define NOHELP
#pragma warning(pop)

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <fstream>
#include <iterator>
#include <list>
#include <locale>
#include <memory>
#include <new>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <wrl/client.h>

#ifdef _WIN32
#include <dxgiformat.h>
#if USE_WIC
#include <wincodec.h>
#endif //USE_WIC
#else //_WIN32
#include <directx/dxgiformat.h>
#endif //_WIN32

#include "tool_util.h"

#pragma warning(disable : 4619 4616 26812)

#include "DirectXTex.h"
#include "DirectXPackedVector.h"

//Uncomment to add support for OpenEXR (.exr)
//#define USE_OPENEXR

#ifdef USE_OPENEXR
// See <https://github.com/Microsoft/DirectXTex/wiki/Adding-OpenEXR> for details
#include "DirectXTexEXR.h"
#endif

using namespace DirectX;
using Microsoft::WRL::ComPtr;

namespace
{
    enum COMMANDS : uint32_t
    {
        CMD_CUBE = 1,
        CMD_VOLUME,
        CMD_ARRAY,
        CMD_CUBEARRAY,
        CMD_H_CROSS,
        CMD_V_CROSS,
        CMD_H_STRIP,
        CMD_V_STRIP,
        CMD_MERGE,
    #if USE_GIF
        CMD_GIF,
    #endif
        CMD_ARRAY_STRIP,
        CMD_MAX
    };

    enum OPTIONS : uint32_t
    {
    #if USE_MULTIPLE_FILES
        OPT_RECURSIVE = 1,
        OPT_FILELIST,
        OPT_WIDTH,
    #else
        OPT_WIDTH = 1,
    #endif
        OPT_HEIGHT,
        OPT_FORMAT,
        OPT_FILTER,
    #if USE_SRGB
        OPT_SRGBI,
        OPT_SRGBO,
        OPT_SRGB,
    #endif
        OPT_OUTPUTFILE,
    #if USE_NAME_CONFIG
        OPT_TOLOWER,
    #endif
        OPT_OVERWRITE,
        OPT_USE_DX10,
    #if USE_LOGO
        OPT_NOLOGO,
    #endif
    #if USE_ALPHA_CONFIG
        OPT_SEPALPHA,
    #endif
    #if USE_WIC
        OPT_NO_WIC,
    #endif
    #if USE_ALPHA_CONFIG
        OPT_DEMUL_ALPHA,
    #endif
        OPT_TA_WRAP,
        OPT_TA_MIRROR,
    #if USE_FEATURE_LEVEL
        OPT_FEATURE_LEVEL,
    #endif
    #if USE_TONEMAP
        OPT_TONEMAP,
    #endif
    #if USE_GIF
        OPT_GIF_BGCOLOR,
    #endif
    #if USE_SWIZZLE
        OPT_SWIZZLE,
    #endif
        OPT_STRIP_MIPS,
        OPT_MAX
    };

    static_assert(OPT_MAX <= 32, "dwOptions is a unsigned int bitfield");

    struct SConversion
    {
        wchar_t szSrc[MAX_PATH];
    };

    struct SValue
    {
        const wchar_t*  name;
        uint32_t        value;
    };

    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////

    const SValue g_pCommands[] =
    {
        { L"cube",          CMD_CUBE },
        { L"volume",        CMD_VOLUME },
        { L"array",         CMD_ARRAY },
        { L"cubearray",     CMD_CUBEARRAY },
        { L"h-cross",       CMD_H_CROSS },
        { L"v-cross",       CMD_V_CROSS },
        { L"h-strip",       CMD_H_STRIP },
        { L"v-strip",       CMD_V_STRIP },
        { L"merge",         CMD_MERGE },
    #if USE_GIF
        { L"gif",           CMD_GIF },
    #endif
        { L"array-strip",   CMD_ARRAY_STRIP },
        { nullptr,          0 }
    };

    const SValue g_pOptions[] =
    {
    #if USE_MULTIPLE_FILES
        { L"r",         OPT_RECURSIVE },
        { L"flist",     OPT_FILELIST },
    #endif
        { L"w",         OPT_WIDTH },
        { L"h",         OPT_HEIGHT },
        { L"f",         OPT_FORMAT },
        { L"if",        OPT_FILTER },
    #if USE_SRGB
        { L"srgbi",     OPT_SRGBI },
        { L"srgbo",     OPT_SRGBO },
        { L"srgb",      OPT_SRGB },
    #endif
        { L"o",         OPT_OUTPUTFILE },
    #if USE_NAME_CONFIG
        { L"l",         OPT_TOLOWER },
    #endif
        { L"y",         OPT_OVERWRITE },
        { L"dx10",      OPT_USE_DX10 },
    #if USE_LOGO
        { L"nologo",    OPT_NOLOGO },
    #endif
    #if USE_ALPHA_CONFIG
        { L"sepalpha",  OPT_SEPALPHA },
    #endif
    #if USE_WIC
        { L"nowic",     OPT_NO_WIC },
    #endif
    #if USE_ALPHA_CONFIG
        { L"alpha",     OPT_DEMUL_ALPHA },
    #endif
        { L"wrap",      OPT_TA_WRAP },
        { L"mirror",    OPT_TA_MIRROR },
    #if USE_FEATURE_LEVEL
        { L"fl",        OPT_FEATURE_LEVEL },
    #endif
    #if USE_TONEMAP
        { L"tonemap",   OPT_TONEMAP },
    #endif
    #if USE_GIF
        { L"bgcolor",   OPT_GIF_BGCOLOR },
    #endif
    #if USE_SWIZZLE
        { L"swizzle",   OPT_SWIZZLE },
    #endif
        { L"stripmips", OPT_STRIP_MIPS },
        { nullptr,      0 }
    };

#define DEFFMT(fmt) { L## #fmt, DXGI_FORMAT_ ## fmt }

    const SValue g_pFormats[] =
    {
        // List does not include _TYPELESS or depth/stencil formats
        DEFFMT(R32G32B32A32_FLOAT),
        DEFFMT(R32G32B32A32_UINT),
        DEFFMT(R32G32B32A32_SINT),
        DEFFMT(R32G32B32_FLOAT),
        DEFFMT(R32G32B32_UINT),
        DEFFMT(R32G32B32_SINT),
        DEFFMT(R16G16B16A16_FLOAT),
        DEFFMT(R16G16B16A16_UNORM),
        DEFFMT(R16G16B16A16_UINT),
        DEFFMT(R16G16B16A16_SNORM),
        DEFFMT(R16G16B16A16_SINT),
        DEFFMT(R32G32_FLOAT),
        DEFFMT(R32G32_UINT),
        DEFFMT(R32G32_SINT),
        DEFFMT(R10G10B10A2_UNORM),
        DEFFMT(R10G10B10A2_UINT),
        DEFFMT(R11G11B10_FLOAT),
        DEFFMT(R8G8B8A8_UNORM),
        DEFFMT(R8G8B8A8_UNORM_SRGB),
        DEFFMT(R8G8B8A8_UINT),
        DEFFMT(R8G8B8A8_SNORM),
        DEFFMT(R8G8B8A8_SINT),
        DEFFMT(R16G16_FLOAT),
        DEFFMT(R16G16_UNORM),
        DEFFMT(R16G16_UINT),
        DEFFMT(R16G16_SNORM),
        DEFFMT(R16G16_SINT),
        DEFFMT(R32_FLOAT),
        DEFFMT(R32_UINT),
        DEFFMT(R32_SINT),
        DEFFMT(R8G8_UNORM),
        DEFFMT(R8G8_UINT),
        DEFFMT(R8G8_SNORM),
        DEFFMT(R8G8_SINT),
        DEFFMT(R16_FLOAT),
        DEFFMT(R16_UNORM),
        DEFFMT(R16_UINT),
        DEFFMT(R16_SNORM),
        DEFFMT(R16_SINT),
        DEFFMT(R8_UNORM),
        DEFFMT(R8_UINT),
        DEFFMT(R8_SNORM),
        DEFFMT(R8_SINT),
        DEFFMT(A8_UNORM),
        //DEFFMT(R1_UNORM)
        DEFFMT(R9G9B9E5_SHAREDEXP),
        DEFFMT(R8G8_B8G8_UNORM),
        DEFFMT(G8R8_G8B8_UNORM),
        DEFFMT(B5G6R5_UNORM),
        DEFFMT(B5G5R5A1_UNORM),

        // DXGI 1.1 formats
        DEFFMT(B8G8R8A8_UNORM),
        DEFFMT(B8G8R8X8_UNORM),
        DEFFMT(R10G10B10_XR_BIAS_A2_UNORM),
        DEFFMT(B8G8R8A8_UNORM_SRGB),
        DEFFMT(B8G8R8X8_UNORM_SRGB),

        // DXGI 1.2 formats
        DEFFMT(AYUV),
        DEFFMT(Y410),
        DEFFMT(Y416),
        DEFFMT(YUY2),
        DEFFMT(Y210),
        DEFFMT(Y216),
        // No support for legacy paletted video formats (AI44, IA44, P8, A8P8)
        DEFFMT(B4G4R4A4_UNORM),

        { nullptr, DXGI_FORMAT_UNKNOWN }
    };

    const SValue g_pFormatAliases[] =
    {
        { L"RGBA", DXGI_FORMAT_R8G8B8A8_UNORM },
        { L"BGRA", DXGI_FORMAT_B8G8R8A8_UNORM },
        { L"BGR",  DXGI_FORMAT_B8G8R8X8_UNORM },

        { L"FP16", DXGI_FORMAT_R16G16B16A16_FLOAT },
        { L"FP32", DXGI_FORMAT_R32G32B32A32_FLOAT },

        { nullptr, DXGI_FORMAT_UNKNOWN }
    };

    const SValue g_pFilters[] =
    {
        { L"POINT",                     TEX_FILTER_POINT },
        { L"LINEAR",                    TEX_FILTER_LINEAR },
        { L"CUBIC",                     TEX_FILTER_CUBIC },
        { L"FANT",                      TEX_FILTER_FANT },
        { L"BOX",                       TEX_FILTER_BOX },
        { L"TRIANGLE",                  TEX_FILTER_TRIANGLE },
        #if USE_DITHER
        { L"POINT_DITHER",              TEX_FILTER_POINT | TEX_FILTER_DITHER },
        { L"LINEAR_DITHER",             TEX_FILTER_LINEAR | TEX_FILTER_DITHER },
        { L"CUBIC_DITHER",              TEX_FILTER_CUBIC | TEX_FILTER_DITHER },
        { L"FANT_DITHER",               TEX_FILTER_FANT | TEX_FILTER_DITHER },
        { L"BOX_DITHER",                TEX_FILTER_BOX | TEX_FILTER_DITHER },
        { L"TRIANGLE_DITHER",           TEX_FILTER_TRIANGLE | TEX_FILTER_DITHER },
        { L"POINT_DITHER_DIFFUSION",    TEX_FILTER_POINT | TEX_FILTER_DITHER_DIFFUSION },
        { L"LINEAR_DITHER_DIFFUSION",   TEX_FILTER_LINEAR | TEX_FILTER_DITHER_DIFFUSION },
        { L"CUBIC_DITHER_DIFFUSION",    TEX_FILTER_CUBIC | TEX_FILTER_DITHER_DIFFUSION },
        { L"FANT_DITHER_DIFFUSION",     TEX_FILTER_FANT | TEX_FILTER_DITHER_DIFFUSION },
        { L"BOX_DITHER_DIFFUSION",      TEX_FILTER_BOX | TEX_FILTER_DITHER_DIFFUSION },
        { L"TRIANGLE_DITHER_DIFFUSION", TEX_FILTER_TRIANGLE | TEX_FILTER_DITHER_DIFFUSION },
        #endif
        { nullptr,                      TEX_FILTER_DEFAULT                              }
    };

#define CODEC_DDS 0xFFFF0001
#define CODEC_TGA 0xFFFF0002
#define CODEC_HDR 0xFFFF0005

#ifdef USE_OPENEXR
#define CODEC_EXR 0xFFFF0006
#endif

    const SValue g_pExtFileTypes[] =
    {
    #if USE_WIC
        { L".BMP",  WIC_CODEC_BMP },
        { L".JPG",  WIC_CODEC_JPEG },
        { L".JPEG", WIC_CODEC_JPEG },
        { L".PNG",  WIC_CODEC_PNG },
    #endif
        { L".DDS",  CODEC_DDS },
        { L".TGA",  CODEC_TGA },
        { L".HDR",  CODEC_HDR },
    #if USE_WIC
        { L".TIF",  WIC_CODEC_TIFF },
        { L".TIFF", WIC_CODEC_TIFF },
        { L".WDP",  WIC_CODEC_WMP },
        { L".HDP",  WIC_CODEC_WMP },
        { L".JXR",  WIC_CODEC_WMP },
    #endif
        #ifdef USE_OPENEXR
        { L"EXR",   CODEC_EXR },
        #endif
        { nullptr,  CODEC_DDS }
    };

    #if USE_FEATURE_LEVEL
    const SValue g_pFeatureLevels[] =   // valid feature levels for -fl for maximimum size
    {
        { L"9.1",  2048 },
        { L"9.2",  2048 },
        { L"9.3",  4096 },
        { L"10.0", 8192 },
        { L"10.1", 8192 },
        { L"11.0", 16384 },
        { L"11.1", 16384 },
        { L"12.0", 16384 },
        { L"12.1", 16384 },
        { L"12.2", 16384 },
        { nullptr, 0 },
    };

    const SValue g_pFeatureLevelsCube[] = // valid feature levels for -fl for maximum cubemap size
    {
        { L"9.1",  512 },
        { L"9.2",  512 },
        { L"9.3",  4096 },
        { L"10.0", 8192 },
        { L"10.1", 8192 },
        { L"11.0", 16384 },
        { L"11.1", 16384 },
        { L"12.0", 16384 },
        { L"12.1", 16384 },
        { L"12.2", 16384 },
        { nullptr, 0 },
    };

    const SValue g_pFeatureLevelsArray[] = // valid feature levels for -fl for maximum array size
    {
        { L"9.1",  1 },
        { L"9.2",  1 },
        { L"9.3",  1 },
        { L"10.0", 512 },
        { L"10.1", 512 },
        { L"11.0", 2048 },
        { L"11.1", 2048 },
        { L"12.0", 2048 },
        { L"12.1", 2048 },
        { L"12.2", 2048 },
        { nullptr, 0 },
    };

    const SValue g_pFeatureLevelsVolume[] = // valid feature levels for -fl for maximum depth size
    {
        { L"9.1",  256 },
        { L"9.2",  256 },
        { L"9.3",  256 },
        { L"10.0", 2048 },
        { L"10.1", 2048 },
        { L"11.0", 2048 },
        { L"11.1", 2048 },
        { L"12.0", 2048 },
        { L"12.1", 2048 },
        { L"12.2", 2048 },
        { nullptr, 0 },
    };
    #endif
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#if USE_GIF
HRESULT LoadAnimatedGif(const wchar_t* szFile,
    std::vector<std::unique_ptr<ScratchImage>>& loadedImages,
    bool usebgcolor);
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace
{
#if USE_MULTIPLE_FILES
    inline HANDLE safe_handle(HANDLE h) noexcept { return (h == INVALID_HANDLE_VALUE) ? nullptr : h; }

    struct find_closer { void operator()(HANDLE h) noexcept { assert(h != INVALID_HANDLE_VALUE); if (h) FindClose(h); } };

    using ScopedFindHandle = std::unique_ptr<void, find_closer>;
#endif

#ifdef _PREFAST_
#pragma prefast(disable : 26018, "Only used with static internal arrays")
#endif

    uint32_t LookupByName(const wchar_t *pName, const SValue *pArray)
    {
        while (pArray->name)
        {
            if (!_wcsicmp(pName, pArray->name))
                return pArray->value;

            pArray++;
        }

        return 0;
    }

#if USE_MULTIPLE_FILES
    void SearchForFiles(const wchar_t* path, std::list<SConversion>& files, bool recursive)
    {
        // Process files
        WIN32_FIND_DATAW findData = {};
        ScopedFindHandle hFile(safe_handle(FindFirstFileExW(path,
            FindExInfoBasic, &findData,
            FindExSearchNameMatch, nullptr,
            FIND_FIRST_EX_LARGE_FETCH)));
        if (hFile)
        {
            for (;;)
            {
                if (!(findData.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY)))
                {
                    wchar_t drive[_MAX_DRIVE] = {};
                    wchar_t dir[_MAX_DIR] = {};
                    _wsplitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, nullptr, 0, nullptr, 0);

                    SConversion conv = {};
                    _wmakepath_s(conv.szSrc, drive, dir, findData.cFileName, nullptr);
                    files.push_back(conv);
                }

                if (!FindNextFileW(hFile.get(), &findData))
                    break;
            }
        }

        // Process directories
        if (recursive)
        {
            wchar_t searchDir[MAX_PATH] = {};
            {
                wchar_t drive[_MAX_DRIVE] = {};
                wchar_t dir[_MAX_DIR] = {};
                _wsplitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, nullptr, 0, nullptr, 0);
                _wmakepath_s(searchDir, drive, dir, L"*", nullptr);
            }

            hFile.reset(safe_handle(FindFirstFileExW(searchDir,
                FindExInfoBasic, &findData,
                FindExSearchLimitToDirectories, nullptr,
                FIND_FIRST_EX_LARGE_FETCH)));
            if (!hFile)
                return;

            for (;;)
            {
                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    if (findData.cFileName[0] != L'.')
                    {
                        wchar_t subdir[MAX_PATH] = {};

                        {
                            wchar_t drive[_MAX_DRIVE] = {};
                            wchar_t dir[_MAX_DIR] = {};
                            wchar_t fname[_MAX_FNAME] = {};
                            wchar_t ext[_MAX_FNAME] = {};
                            _wsplitpath_s(path, drive, dir, fname, ext);
                            wcscat_s(dir, findData.cFileName);
                            _wmakepath_s(subdir, drive, dir, fname, ext);
                        }

                        SearchForFiles(subdir, files, recursive);
                    }
                }

                if (!FindNextFileW(hFile.get(), &findData))
                    break;
            }
        }
    }

    void ProcessFileList(std::wifstream& inFile, std::list<SConversion>& files)
    {
        std::list<SConversion> flist;
        std::set<std::wstring> excludes;
        wchar_t fname[1024] = {};
        for (;;)
        {
            inFile >> fname;
            if (!inFile)
                break;

            if (*fname == L'#')
            {
                // Comment
            }
            else if (*fname == L'-')
            {
                if (flist.empty())
                {
                    wprintf(L"WARNING: Ignoring the line '%ls' in -flist\n", fname);
                }
                else
                {
                    if (wcspbrk(fname, L"?*") != nullptr)
                    {
                        std::list<SConversion> removeFiles;
                        SearchForFiles(&fname[1], removeFiles, false);

                        for (auto it : removeFiles)
                        {
                            _wcslwr_s(it.szSrc);
                            excludes.insert(it.szSrc);
                        }
                    }
                    else
                    {
                        std::wstring name = (fname + 1);
                        std::transform(name.begin(), name.end(), name.begin(), towlower);
                        excludes.insert(name);
                    }
                }
            }
            else if (wcspbrk(fname, L"?*") != nullptr)
            {
                SearchForFiles(fname, flist, false);
            }
            else
            {
                SConversion conv = {};
                wcscpy_s(conv.szSrc, MAX_PATH, fname);
                flist.push_back(conv);
            }

            inFile.ignore(1000, '\n');
        }

        inFile.close();

        if (!excludes.empty())
        {
            // Remove any excluded files
            for (auto it = flist.begin(); it != flist.end();)
            {
                std::wstring name = it->szSrc;
                std::transform(name.begin(), name.end(), name.begin(), towlower);
                auto item = it;
                ++it;
                if (excludes.find(name) != excludes.end())
                {
                    flist.erase(item);
                }
            }
        }

        if (flist.empty())
        {
            wprintf(L"WARNING: No file names found in -flist\n");
        }
        else
        {
            files.splice(files.end(), flist);
        }
    }
#endif

#if USE_PRINT_INFO
    void PrintFormat(DXGI_FORMAT Format)
    {
        for (auto pFormat = g_pFormats; pFormat->name; pFormat++)
        {
            if (static_cast<DXGI_FORMAT>(pFormat->value) == Format)
            {
                wprintf(L"%ls", pFormat->name);
                break;
            }
        }
    }

    void PrintInfo(const TexMetadata& info)
    {
        wprintf(L" (%zux%zu", info.width, info.height);

        if (TEX_DIMENSION_TEXTURE3D == info.dimension)
            wprintf(L"x%zu", info.depth);

        if (info.mipLevels > 1)
            wprintf(L",%zu", info.mipLevels);

        if (info.arraySize > 1)
            wprintf(L",%zu", info.arraySize);

        wprintf(L" ");
        PrintFormat(info.format);

        switch (info.dimension)
        {
        case TEX_DIMENSION_TEXTURE1D:
            wprintf(L"%ls", (info.arraySize > 1) ? L" 1DArray" : L" 1D");
            break;

        case TEX_DIMENSION_TEXTURE2D:
            if (info.IsCubemap())
            {
                wprintf(L"%ls", (info.arraySize > 6) ? L" CubeArray" : L" Cube");
            }
            else
            {
                wprintf(L"%ls", (info.arraySize > 1) ? L" 2DArray" : L" 2D");
            }
            break;

        case TEX_DIMENSION_TEXTURE3D:
            wprintf(L" 3D");
            break;
        }

        switch (info.GetAlphaMode())
        {
        case TEX_ALPHA_MODE_OPAQUE:
            wprintf(L" \x0e0:Opaque");
            break;
        case TEX_ALPHA_MODE_PREMULTIPLIED:
            wprintf(L" \x0e0:PM");
            break;
        case TEX_ALPHA_MODE_STRAIGHT:
            wprintf(L" \x0e0:NonPM");
            break;
        case TEX_ALPHA_MODE_CUSTOM:
            wprintf(L" \x0e0:Custom");
            break;
        case TEX_ALPHA_MODE_UNKNOWN:
            break;
        }

        wprintf(L")");
    }
#endif

#if USE_USAGE
    void PrintList(size_t cch, const SValue *pValue)
    {
        while (pValue->name)
        {
            const size_t cchName = wcslen(pValue->name);

            if (cch + cchName + 2 >= 80)
            {
                wprintf(L"\n      ");
                cch = 6;
            }

            wprintf(L"%ls ", pValue->name);
            cch += cchName + 2;
            pValue++;
        }

        wprintf(L"\n");
    }
#endif

#if USE_LOGO
    void PrintLogo()
    {
        wchar_t version[32] = {};

        wchar_t appName[_MAX_PATH] = {};
        if (GetModuleFileNameW(nullptr, appName, static_cast<DWORD>(std::size(appName))))
        {
            const DWORD size = GetFileVersionInfoSizeW(appName, nullptr);
            if (size > 0)
            {
                auto verInfo = std::make_unique<uint8_t[]>(size);
                if (GetFileVersionInfoW(appName, 0, size, verInfo.get()))
                {
                    LPVOID lpstr = nullptr;
                    UINT strLen = 0;
                    if (VerQueryValueW(verInfo.get(), L"\\StringFileInfo\\040904B0\\ProductVersion", &lpstr, &strLen))
                    {
                        wcsncpy_s(version, reinterpret_cast<const wchar_t*>(lpstr), strLen);
                    }
                }
            }
        }

        if (!*version || wcscmp(version, L"1.0.0.0") == 0)
        {
            swprintf_s(version, L"%03d (library)", DIRECTX_TEX_VERSION);
        }

        wprintf(L"Microsoft (R) DirectX Texture Assembler [DirectXTex] Version %ls\n", version);
        wprintf(L"Copyright (C) Microsoft Corp.\n");
    #ifdef _DEBUG
        wprintf(L"*** Debug build ***\n");
    #endif
        wprintf(L"\n");
    }
#endif

#if USE_USAGE
    void PrintUsage()
    {
    #if USE_LOGO
        PrintLogo();
    #endif

        wprintf(L"Usage: texassemble <command> <options> <files>\n\n");
        wprintf(L"   cube                create cubemap\n");
        wprintf(L"   volume              create volume map\n");
        wprintf(L"   array               create texture array\n");
        wprintf(L"   cubearray           create cubemap array\n");
        wprintf(L"   h-cross or v-cross  create a cross image from a cubemap\n");
        wprintf(L"   h-strip or v-strip  create a strip image from a cubemap\n");
        wprintf(L"   array-strip         creates a strip image from a 1D/2D array\n");
        wprintf(L"   merge               create texture from rgb image and alpha image\n");
    #if USE_GIF
        wprintf(L"   gif                 create array from animated gif\n\n");
    #endif
    #if USE_MULTIPLE_FILES
        wprintf(L"   -r                  wildcard filename search is recursive\n");
        wprintf(L"   -flist <filename>   use text file with a list of input files (one per line)\n");
    #endif
        wprintf(L"   -w <n>              width\n");
        wprintf(L"   -h <n>              height\n");
        wprintf(L"   -f <format>         format\n");
        wprintf(L"   -if <filter>        image filtering\n");
    #if USE_SRGB
        wprintf(L"   -srgb{i|o}          sRGB {input, output}\n");
    #endif
        wprintf(L"   -o <filename>       output filename\n");
        wprintf(L"   -l                  force output filename to lower case\n");
        wprintf(L"   -y                  overwrite existing output file (if any)\n");
    #if USE_ALPHA_CONFIG
        wprintf(L"   -sepalpha           resize alpha channel separately from color channels\n");
    #endif
    #if USE_WIC
        wprintf(L"   -nowic              Force non-WIC filtering\n");
    #endif
        wprintf(L"   -wrap, -mirror      texture addressing mode (wrap, mirror, or clamp)\n");
    #if USE_ALPHA_CONFIG
        wprintf(L"   -alpha              convert premultiplied alpha to straight alpha\n");
    #endif
        wprintf(L"   -dx10               Force use of 'DX10' extended header\n");
    #if USE_LOGO
        wprintf(L"   -nologo             suppress copyright message\n");
    #endif
    #if USE_FEATURE_LEVEL
        wprintf(L"   -fl <feature-level> Set maximum feature level target (defaults to 11.0)\n");
    #endif
    #if USE_TONEMAP
        wprintf(L"   -tonemap            Apply a tonemap operator based on maximum luminance\n");
    #endif
    #if USE_GIF
        wprintf(L"\n                       (gif only)\n");
        wprintf(L"   -bgcolor            Use background color instead of transparency\n");
    #endif
    #if USE_SWIZZLE
        wprintf(L"\n                       (merge only)\n");
        wprintf(L"   -swizzle <rgba>     Select channels for merge (defaults to rgbB)\n");
    #endif
        wprintf(L"\n                       (cube, volume, array, cubearray, merge only)\n");
        wprintf(L"   -stripmips          Use only base image from input dds files\n");

        wprintf(L"\n   <format>: ");
        PrintList(13, g_pFormats);
        wprintf(L"      ");
        PrintList(13, g_pFormatAliases);

        wprintf(L"\n   <filter>: ");
        PrintList(13, g_pFilters);
    #if USE_FEATURE_LEVEL
        wprintf(L"\n   <feature-level>: ");
        PrintList(13, g_pFeatureLevels);
    #endif
    }
#endif //USE_USAGE

    HRESULT SaveImageFile(const Image& img, uint32_t fileType, const wchar_t* szOutputFile)
    {
        switch (fileType)
        {
        case CODEC_DDS:
            return SaveToDDSFile(img, DDS_FLAGS_NONE, szOutputFile);

        case CODEC_TGA:
            return SaveToTGAFile(img, TGA_FLAGS_NONE, szOutputFile);

        case CODEC_HDR:
            return SaveToHDRFile(img, szOutputFile);

        #ifdef USE_OPENEXR
        case CODEC_EXR:
            return SaveToEXRFile(img, szOutputFile);
        #endif

        default:
            {
            #if USE_WIC
                HRESULT hr = SaveToWICFile(img, WIC_FLAGS_NONE, GetWICCodec(static_cast<WICCodecs>(fileType)), szOutputFile);
                if ((hr == static_cast<HRESULT>(0xc00d5212) /* MF_E_TOPO_CODEC_NOT_FOUND */) && (fileType == WIC_CODEC_HEIF))
                {
                    wprintf(L"\nINFO: This format requires installing the HEIF Image Extensions - https://aka.ms/heif\n");
                }
                return hr;
            #else
                return 0x80004005;
            #endif
            }
        }
    }

#if USE_SWIZZLE
    bool ParseSwizzleMask(
        _In_reads_(4) const wchar_t* mask,
        _Out_writes_(4) uint32_t* permuteElements,
        _Out_writes_(4) uint32_t* zeroElements,
        _Out_writes_(4) uint32_t* oneElements)
    {
        if (!mask || !permuteElements || !zeroElements || !oneElements)
            return false;

        if (!mask[0])
            return false;

        for (uint32_t j = 0; j < 4; ++j)
        {
            if (!mask[j])
                break;

            switch (mask[j])
            {
            case L'r':
            case L'x':
                for (uint32_t k = j; k < 4; ++k)
                {
                    permuteElements[k] = 0;
                    zeroElements[k] = 0;
                    oneElements[k] = 0;
                }
                break;

            case L'R':
            case L'X':
                for (uint32_t k = j; k < 4; ++k)
                {
                    permuteElements[k] = 4;
                    zeroElements[k] = 0;
                    oneElements[k] = 0;
                }
                break;

            case L'g':
            case L'y':
                for (uint32_t k = j; k < 4; ++k)
                {
                    permuteElements[k] = 1;
                    zeroElements[k] = 0;
                    oneElements[k] = 0;
                }
                break;

            case L'G':
            case L'Y':
                for (uint32_t k = j; k < 4; ++k)
                {
                    permuteElements[k] = 5;
                    zeroElements[k] = 0;
                    oneElements[k] = 0;
                }
                break;

            case L'b':
            case L'z':
                for (uint32_t k = j; k < 4; ++k)
                {
                    permuteElements[k] = 2;
                    zeroElements[k] = 0;
                    oneElements[k] = 0;
                }
                break;

            case L'B':
            case L'Z':
                for (uint32_t k = j; k < 4; ++k)
                {
                    permuteElements[k] = 6;
                    zeroElements[k] = 0;
                    oneElements[k] = 0;
                }
                break;

            case L'a':
            case L'w':
                for (uint32_t k = j; k < 4; ++k)
                {
                    permuteElements[k] = 3;
                    zeroElements[k] = 0;
                    oneElements[k] = 0;
                }
                break;

            case L'A':
            case L'W':
                for (uint32_t k = j; k < 4; ++k)
                {
                    permuteElements[k] = 7;
                    zeroElements[k] = 0;
                    oneElements[k] = 0;
                }
                break;

            case L'0':
                for (uint32_t k = j; k < 4; ++k)
                {
                    permuteElements[k] = k;
                    zeroElements[k] = 1;
                    oneElements[k] = 0;
                }
                break;

            case L'1':
                for (uint32_t k = j; k < 4; ++k)
                {
                    permuteElements[k] = k;
                    zeroElements[k] = 0;
                    oneElements[k] = 1;
                }
                break;

            default:
                return false;
            }
        }

        return true;
    }
#endif
}

//--------------------------------------------------------------------------------------
// Entry-point
//--------------------------------------------------------------------------------------
#ifdef _PREFAST_
#pragma prefast(disable : 28198, "Command-line tool, frees all memory on exit")
#endif

//int __cdecl wmain(_In_ int argc, _In_z_count_(argc) wchar_t* argv[]) {
#ifdef _WIN32
extern "C" __declspec(dllexport) int __cdecl texassemble(int argc, wchar_t* argv[], bool verbose = true, bool init_com = false, wchar_t* err_buf = nullptr, int err_buf_size = 0)
{
#else
extern "C" __attribute__((visibility("default"))) int texassemble(int argc, wchar_t* argv[], bool verbose = true, bool init_com = false, wchar_t* err_buf = nullptr, int err_buf_size = 0)
{
    init_com = false;
#endif
    // Parameters and defaults
    size_t width = 0;
    size_t height = 0;

    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
    TEX_FILTER_FLAGS dwFilter = TEX_FILTER_DEFAULT;
    TEX_FILTER_FLAGS dwSRGB = TEX_FILTER_DEFAULT;
    TEX_FILTER_FLAGS dwFilterOpts = TEX_FILTER_DEFAULT;
    #if USE_WIC
    uint32_t fileType = WIC_CODEC_BMP;
    #else
    uint32_t fileType = CODEC_TGA;
    #endif
    uint32_t maxSize = 16384;
    uint32_t maxCube = 16384;
    uint32_t maxArray = 2048;
    uint32_t maxVolume = 2048;

    // DXTex's Open Alpha onto Surface always loaded alpha from the blue channel
    uint32_t permuteElements[4] = { 0, 1, 2, 6 };
    uint32_t zeroElements[4] = {};
    uint32_t oneElements[4] = {};

    wchar_t szOutputFile[MAX_PATH] = {};

    // Set locale for output since GetErrorDesc can get localized strings.
    std::locale::global(std::locale(""));

    #if USE_WIC
    // Initialize COM (needed for WIC)
    if (init_com)
    {
        HRESULT hr = hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (FAILED(hr))
        {
            RaiseErrorCodeMessage(err_buf, err_buf_size, L"Failed to initialize COM (", hr);
            return 1;
        }
    }
    #else
    HRESULT hr;
    #endif

    // Process command line
    if (argc < 1)
    {
    #if USE_USAGE
        PrintUsage();
    #endif
        return 0;
    }

    const uint32_t dwCommand = LookupByName(argv[0], g_pCommands);
    switch (dwCommand)
    {
    case CMD_CUBE:
    case CMD_VOLUME:
    case CMD_ARRAY:
    case CMD_CUBEARRAY:
    case CMD_H_CROSS:
    case CMD_V_CROSS:
    case CMD_H_STRIP:
    case CMD_V_STRIP:
    case CMD_MERGE:
#if USE_GIF
    case CMD_GIF:
#endif
    case CMD_ARRAY_STRIP:
        break;

    default:
        RaiseErrorMessage(err_buf, err_buf_size, L"Must use one of: cube, volume, array, cubearray,\n   h-cross, v-cross, h-strip, v-strip, array-strip\n   merge, gif\n\n");
        return 1;
    }

    uint32_t dwOptions = 0;
    std::list<SConversion> conversion;

    for (int iArg = 1; iArg < argc; iArg++)
    {
        PWSTR pArg = argv[iArg];

        //if (('-' == pArg[0]) || ('/' == pArg[0]))
        if ('-' == pArg[0])  // '/' is used for paths in Unix systems.
        {
            pArg++;
            PWSTR pValue;

            for (pValue = pArg; *pValue && (':' != *pValue); pValue++);

            if (*pValue)
                *pValue++ = 0;

            const uint32_t dwOption = LookupByName(pArg, g_pOptions);

        #if USE_USAGE
            if (!dwOption || (dwOptions & (uint64_t(1) << dwOption)))
            {
                PrintUsage();
                return 0;
            }
        #endif

            dwOptions |= 1 << dwOption;

            // Handle options with additional value parameter
            switch (dwOption)
            {
            #if USE_MULTIPLE_FILES
            case OPT_FILELIST:
            #endif
            case OPT_WIDTH:
            case OPT_HEIGHT:
            case OPT_FORMAT:
            case OPT_FILTER:
            case OPT_OUTPUTFILE:
            #if USE_FEATURE_LEVEL
            case OPT_FEATURE_LEVEL:
            #endif
            #if USE_SWIZZLE
            case OPT_SWIZZLE:
            #endif
                if (!*pValue)
                {
                    if ((iArg + 1 >= argc))
                    {
                        RaiseInvalidOptionError(err_buf, err_buf_size, L"Failed to parse arguments.\n");
                        return 1;
                    }

                    iArg++;
                    pValue = argv[iArg];
                }
                break;

            default:
                break;
            }

            switch (dwOption)
            {
            case OPT_WIDTH:
                if (swscanf_s(pValue, L"%zu", &width) != 1)
                {
                    RaiseInvalidOptionError(err_buf, err_buf_size, L"-w", pValue);
                    return 1;
                }
                break;

            case OPT_HEIGHT:
                if (swscanf_s(pValue, L"%zu", &height) != 1)
                {
                    RaiseInvalidOptionError(err_buf, err_buf_size, L"-h", pValue);
                    return 1;
                }
                break;

            case OPT_FORMAT:
                format = static_cast<DXGI_FORMAT>(LookupByName(pValue, g_pFormats));
                if (!format)
                {
                    format = static_cast<DXGI_FORMAT>(LookupByName(pValue, g_pFormatAliases));
                    if (!format)
                    {
                       RaiseInvalidOptionError(err_buf, err_buf_size, L"-f", pValue);
                        return 1;
                    }
                }
                break;

            case OPT_FILTER:
                dwFilter = static_cast<TEX_FILTER_FLAGS>(LookupByName(pValue, g_pFilters));
                if (!dwFilter)
                {
                    RaiseInvalidOptionError(err_buf, err_buf_size, L"-if", pValue);
                    return 1;
                }
                break;

        #if USE_SRGB
            case OPT_SRGBI:
                dwSRGB |= TEX_FILTER_SRGB_IN;
                break;

            case OPT_SRGBO:
                dwSRGB |= TEX_FILTER_SRGB_OUT;
                break;

            case OPT_SRGB:
                dwSRGB |= TEX_FILTER_SRGB;
                break;
        #endif

        #if USE_ALPHA_CONFIG
            case OPT_SEPALPHA:
                dwFilterOpts |= TEX_FILTER_SEPARATE_ALPHA;
                break;
        #endif

        #if USE_WIC
            case OPT_NO_WIC:
                dwFilterOpts |= TEX_FILTER_FORCE_NON_WIC;
                break;
        #endif

            case OPT_OUTPUTFILE:
                {
                    wcscpy_s(szOutputFile, MAX_PATH, pValue);

                    wchar_t ext[_MAX_EXT] = {};
                    _wsplitpath_s(szOutputFile, nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

                    fileType = LookupByName(ext, g_pExtFileTypes);

                    switch (dwCommand)
                    {
                    case CMD_H_CROSS:
                    case CMD_V_CROSS:
                    case CMD_H_STRIP:
                    case CMD_V_STRIP:
                    case CMD_MERGE:
                    case CMD_ARRAY_STRIP:
                        break;

                    default:
                        if (fileType != CODEC_DDS)
                        {
                            RaiseErrorMessage(err_buf, err_buf_size, L"Assembled output file must be a dds\n");
                            return 1;
                        }
                    }
                    break;
                }

            case OPT_TA_WRAP:
                if (dwFilterOpts & TEX_FILTER_MIRROR)
                {
                    RaiseInvalidOptionError(err_buf, err_buf_size, L"Can't use -wrap and -mirror at same time\n");
                    return 1;
                }
                dwFilterOpts |= TEX_FILTER_WRAP;
                break;

            case OPT_TA_MIRROR:
                if (dwFilterOpts & TEX_FILTER_WRAP)
                {
                    RaiseInvalidOptionError(err_buf, err_buf_size, L"Can't use -wrap and -mirror at same time\n");
                    return 1;
                }
                dwFilterOpts |= TEX_FILTER_MIRROR;
                break;

            #if USE_MULTIPLE_FILES
            case OPT_FILELIST:
                {
                    std::wifstream inFile(pValue);
                    if (!inFile)
                    {
                        RaiseErrorMessage(err_buf, err_buf_size, L"Error opening -flist file ", pValue, L"\n");
                        return 1;
                    }

                    inFile.imbue(std::locale::classic());

                    ProcessFileList(inFile, conversion);
                }
                break;
            #endif

            #if USE_FEATURE_LEVEL
            case OPT_FEATURE_LEVEL:
                maxSize = LookupByName(pValue, g_pFeatureLevels);
                maxCube = LookupByName(pValue, g_pFeatureLevelsCube);
                maxArray = LookupByName(pValue, g_pFeatureLevelsArray);
                maxVolume = LookupByName(pValue, g_pFeatureLevelsVolume);
                if (!maxSize || !maxCube || !maxArray || !maxVolume)
                {
                    RaiseInvalidOptionError(err_buf, err_buf_size, L"-fl", pValue)
                    return 1;
                }
                break;
            #endif

            #if USE_GIF
            case OPT_GIF_BGCOLOR:
                if (dwCommand != CMD_GIF)
                {
                    RaiseErrorMessage(err_buf, err_buf_size, L"-bgcolor only applies to gif command\n");
                    return 1;
                }
                break;
            #endif

            #if USE_SWIZZLE
            case OPT_SWIZZLE:
                if (dwCommand != CMD_MERGE)
                {
                    RaiseErrorMessage(err_buf, err_buf_size, L"-swizzle only applies to merge command\n");
                    return 1;
                }
                if (!*pValue || wcslen(pValue) > 4)
                {
                    RaiseInvalidOptionError(err_buf, err_buf_size, L"-swizzle", pValue)
                    return 1;
                }
                else if (!ParseSwizzleMask(pValue, permuteElements, zeroElements, oneElements))
                {
                    RaiseErrorMessage(err_buf, err_buf_size, L"-swizzle requires a 1 to 4 character mask composed of these letters: r, g, b, a, x, y, w, z, 0, 1.\n    Lowercase letters are from the first image, upper-case letters are from the second image.\n");
                    return 1;
                }
                break;
            #endif

            case OPT_STRIP_MIPS:
                switch (dwCommand)
                {
                case CMD_CUBE:
                case CMD_VOLUME:
                case CMD_ARRAY:
                case CMD_CUBEARRAY:
                case CMD_MERGE:
                    break;

                default:
                    RaiseErrorMessage(err_buf, err_buf_size, L"-stripmips only applies to cube, volume, array, cubearray, or merge commands\n");
                    return 1;
                }
                break;

            default:
                break;
            }
        }
        #if USE_MULTIPLE_FILES
        else if (wcspbrk(pArg, L"?*") != nullptr)
        {
            const size_t count = conversion.size();
            SearchForFiles(pArg, conversion, (dwOptions & (1 << OPT_RECURSIVE)) != 0);
            if (conversion.size() <= count)
            {
                RaiseErrorMessage(err_buf, err_buf_size, L"No matching files found for ", pArg, L"\n");
                return 1;
            }
        }
        #endif
        else
        {
            SConversion conv = {};
            wcscpy_s(conv.szSrc, MAX_PATH, pArg);

            conversion.push_back(conv);
        }
    }

    if (conversion.empty())
    {
    #if USE_USAGE
        PrintUsage();
    #endif
        return 0;
    }

#if USE_LOGO
    if (~dwOptions & (1 << OPT_NOLOGO))
        PrintLogo();
#endif

    switch (dwCommand)
    {
    case CMD_H_CROSS:
    case CMD_V_CROSS:
    case CMD_H_STRIP:
    case CMD_V_STRIP:
#if USE_GIF
    case CMD_GIF:
#endif
    case CMD_ARRAY_STRIP:
        if (conversion.size() > 1)
        {
            RaiseErrorMessage(err_buf, err_buf_size, L"ERROR: cross/strip/gif output only accepts 1 input file\n");
            return 1;
        }
        break;

    case CMD_MERGE:
        if (conversion.size() > 2)
        {
            RaiseErrorMessage(err_buf, err_buf_size, L"ERROR: merge output only accepts 2 input files\n");
            return 1;
        }
        break;

    default:
        break;
    }

    // Convert images
    size_t images = 0;

    std::vector<std::unique_ptr<ScratchImage>> loadedImages;

#if USE_GIF
    if (dwCommand == CMD_GIF)
    {
        wchar_t ext[_MAX_EXT] = {};
        wchar_t fname[_MAX_FNAME] = {};
        _wsplitpath_s(conversion.front().szSrc, nullptr, 0, nullptr, 0, fname, _MAX_FNAME, ext, _MAX_EXT);

        if (verbose) {
        wprintf(L"reading %ls", conversion.front().szSrc);
        }
        fflush(stdout);

        if (!*szOutputFile)
        {
            _wmakepath_s(szOutputFile, nullptr, nullptr, fname, L".dds");
        }

        hr = LoadAnimatedGif(conversion.front().szSrc, loadedImages, (dwOptions & (1 << OPT_GIF_BGCOLOR)) != 0);
        if (FAILED(hr))
        {
            RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED (", hr);
            return 1;
        }
    }
    else
#endif
    {
        for (auto pConv = conversion.begin(); pConv != conversion.end(); ++pConv)
        {
            wchar_t ext[_MAX_EXT] = {};
            wchar_t fname[_MAX_FNAME] = {};
            _wsplitpath_s(pConv->szSrc, nullptr, 0, nullptr, 0, fname, _MAX_FNAME, ext, _MAX_EXT);

            // Load source image
            if (pConv != conversion.begin() && verbose)
                wprintf(L"\n");
            else if (!*szOutputFile)
            {
                switch (dwCommand)
                {
                case CMD_H_CROSS:
                case CMD_V_CROSS:
                case CMD_H_STRIP:
                case CMD_V_STRIP:
                case CMD_ARRAY_STRIP:
                    _wmakepath_s(szOutputFile, nullptr, nullptr, fname, L".bmp");
                    break;

                default:
                    if (_wcsicmp(ext, L".dds") == 0)
                    {
                        RaiseErrorMessage(err_buf, err_buf_size, L"ERROR: Need to specify output file via -o\n");
                        return 1;
                    }

                    _wmakepath_s(szOutputFile, nullptr, nullptr, fname, L".dds");
                    break;
                }
            }
            if (verbose) {
                wprintf(L"reading %ls", pConv->szSrc);
            }
            fflush(stdout);

            TexMetadata info;
            std::unique_ptr<ScratchImage> image(new (std::nothrow) ScratchImage);
            if (!image)
            {
                RaiseMemoryAllocError(err_buf, err_buf_size);
                return 1;
            }

            switch (dwCommand)
            {
            case CMD_H_CROSS:
            case CMD_V_CROSS:
            case CMD_H_STRIP:
            case CMD_V_STRIP:
                if (_wcsicmp(ext, L".dds") == 0)
                {
                    hr = LoadFromDDSFile(pConv->szSrc, DDS_FLAGS_ALLOW_LARGE_FILES, &info, *image);
                    if (FAILED(hr))
                    {
                        RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED (", hr);
                        return 1;
                    }

                    if (!info.IsCubemap())
                    {
                        wprintf(L"\n");
                        RaiseErrorMessage(err_buf, err_buf_size, L"ERROR: Input must be a cubemap\n");
                        return 1;
                    }
                    else if (info.arraySize != 6)
                    {
                        wprintf(L"\nWARNING: Only the first cubemap in an array is written out as a cross/strip\n");
                    }
                }
                else
                {
                    wprintf(L"\n");
                    RaiseErrorMessage(err_buf, err_buf_size, L"ERROR: Input must be a dds of a cubemap\n");
                    return 1;
                }
                break;

            case CMD_ARRAY_STRIP:
                if (_wcsicmp(ext, L".dds") == 0)
                {
                    hr = LoadFromDDSFile(pConv->szSrc, DDS_FLAGS_ALLOW_LARGE_FILES, &info, *image);
                    if (FAILED(hr))
                    {
                        RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED (", hr);
                        return 1;
                    }

                    if (info.dimension == TEX_DIMENSION_TEXTURE3D || info.arraySize < 2 || info.IsCubemap())
                    {
                        wprintf(L"\n");
                        RaiseErrorMessage(err_buf, err_buf_size, L"ERROR: Input must be a 1D/2D array\n");
                        return 1;
                    }
                }
                else
                {
                    wprintf(L"\n");
                    RaiseErrorMessage(err_buf, err_buf_size, L"ERROR: Input must be a dds of a 1D/2D array\n");
                    return 1;
                }
                break;

            default:
                if (_wcsicmp(ext, L".dds") == 0)
                {
                    hr = LoadFromDDSFile(pConv->szSrc, DDS_FLAGS_ALLOW_LARGE_FILES, &info, *image);
                    if (FAILED(hr))
                    {
                        RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED (", hr);
                        return 1;
                    }

                    if (info.IsVolumemap() || info.IsCubemap())
                    {
                        wprintf(L"\n");
                        RaiseErrorMessage(err_buf, err_buf_size, L"ERROR: Can't assemble complex surfaces\n");
                        return 1;
                    }
                    else if ((info.mipLevels > 1) && ((dwOptions & (1 << OPT_STRIP_MIPS)) == 0))
                    {
                        wprintf(L"\n");
                        RaiseErrorMessage(err_buf, err_buf_size, L"ERROR: Can't assemble using input mips. To ignore mips, try again with -stripmips\n");
                        return 1;
                    }
                }
                else if (_wcsicmp(ext, L".tga") == 0)
                {
                    TGA_FLAGS tgaFlags = (IsBGR(format)) ? TGA_FLAGS_BGR : TGA_FLAGS_NONE;

                    hr = LoadFromTGAFile(pConv->szSrc, tgaFlags, &info, *image);
                    if (FAILED(hr))
                    {
                        RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED (", hr);
                        return 1;
                    }
                }
                else if (_wcsicmp(ext, L".hdr") == 0)
                {
                    hr = LoadFromHDRFile(pConv->szSrc, &info, *image);
                    if (FAILED(hr))
                    {
                        RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED (", hr);
                        return 1;
                    }
                }
            #ifdef USE_OPENEXR
                else if (_wcsicmp(ext, L".exr") == 0)
                {
                    hr = LoadFromEXRFile(pConv->szSrc, &info, *image);
                    if (FAILED(hr))
                    {
                        RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED (", hr);
                        return 1;
                    }
                }
            #endif
                else
                {
                    #if USE_WIC
                    // WIC shares the same filter values for mode and dither
                    #if USE_DITHER
                    static_assert(static_cast<int>(WIC_FLAGS_DITHER) == static_cast<int>(TEX_FILTER_DITHER), "WIC_FLAGS_* & TEX_FILTER_* should match");
                    static_assert(static_cast<int>(WIC_FLAGS_DITHER_DIFFUSION) == static_cast<int>(TEX_FILTER_DITHER_DIFFUSION), "WIC_FLAGS_* & TEX_FILTER_* should match");
                    #endif
                    static_assert(static_cast<int>(WIC_FLAGS_FILTER_POINT) == static_cast<int>(TEX_FILTER_POINT), "WIC_FLAGS_* & TEX_FILTER_* should match");
                    static_assert(static_cast<int>(WIC_FLAGS_FILTER_LINEAR) == static_cast<int>(TEX_FILTER_LINEAR), "WIC_FLAGS_* & TEX_FILTER_* should match");
                    static_assert(static_cast<int>(WIC_FLAGS_FILTER_CUBIC) == static_cast<int>(TEX_FILTER_CUBIC), "WIC_FLAGS_* & TEX_FILTER_* should match");
                    static_assert(static_cast<int>(WIC_FLAGS_FILTER_FANT) == static_cast<int>(TEX_FILTER_FANT), "WIC_FLAGS_* & TEX_FILTER_* should match");

                    hr = LoadFromWICFile(pConv->szSrc, WIC_FLAGS_ALL_FRAMES | dwFilter, &info, *image);
                    if (FAILED(hr))
                    {
                        RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED (", hr);
                        if (hr == static_cast<HRESULT>(0xc00d5212) /* MF_E_TOPO_CODEC_NOT_FOUND */)
                        {
                            if (_wcsicmp(ext, L".heic") == 0 || _wcsicmp(ext, L".heif") == 0)
                            {
                                wprintf(L"INFO: This format requires installing the HEIF Image Extensions - https://aka.ms/heif\n");
                            }
                            else if (_wcsicmp(ext, L".webp") == 0)
                            {
                                wprintf(L"INFO: This format requires installing the WEBP Image Extensions - https://www.microsoft.com/p/webp-image-extensions/9pg2dk419drg\n");
                            }
                        }
                        return 1;
                    }
                    #else
                    RaiseErrorMessage(err_buf, err_buf_size, L"WIC is unsupported.\n");
                    return 1;
                    #endif
                }
                break;
            }
        
        #if USE_PRINT_INFO
            PrintInfo(info);
        #endif

            // Convert texture
            fflush(stdout);

            // --- Planar ------------------------------------------------------------------
            if (IsPlanar(info.format))
            {
                auto img = image->GetImage(0, 0, 0);
                assert(img);
                const size_t nimg = image->GetImageCount();

                std::unique_ptr<ScratchImage> timage(new (std::nothrow) ScratchImage);
                if (!timage)
                {
                    RaiseMemoryAllocError(err_buf, err_buf_size);
                    return 1;
                }

                hr = ConvertToSinglePlane(img, nimg, info, *timage);
                if (FAILED(hr))
                {
                    RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED [converttosingleplane] (", hr);
                    return 1;
                }

                auto& tinfo = timage->GetMetadata();

                info.format = tinfo.format;

                assert(info.width == tinfo.width);
                assert(info.height == tinfo.height);
                assert(info.depth == tinfo.depth);
                assert(info.arraySize == tinfo.arraySize);
                assert(info.mipLevels == tinfo.mipLevels);
                assert(info.miscFlags == tinfo.miscFlags);
                assert(info.dimension == tinfo.dimension);

                image.swap(timage);
            }

            // --- Decompress --------------------------------------------------------------
            if (IsCompressed(info.format))
            {
                const Image* img = image->GetImage(0, 0, 0);
                assert(img);
                const size_t nimg = image->GetImageCount();

                std::unique_ptr<ScratchImage> timage(new (std::nothrow) ScratchImage);
                if (!timage)
                {
                    RaiseMemoryAllocError(err_buf, err_buf_size);
                    return 1;
                }

                hr = Decompress(img, nimg, info, DXGI_FORMAT_UNKNOWN /* picks good default */, *timage.get());
                if (FAILED(hr))
                {
                    RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED [decompress] (", hr);
                    return 1;
                }

                auto& tinfo = timage->GetMetadata();

                info.format = tinfo.format;

                assert(info.width == tinfo.width);
                assert(info.height == tinfo.height);
                assert(info.depth == tinfo.depth);
                assert(info.arraySize == tinfo.arraySize);
                assert(info.mipLevels == tinfo.mipLevels);
                assert(info.miscFlags == tinfo.miscFlags);
                assert(info.dimension == tinfo.dimension);

                image.swap(timage);
            }

            // --- Strip Mips (if requested) -----------------------------------------------
            if ((info.mipLevels > 1) && (dwOptions & (1 << OPT_STRIP_MIPS)))
            {
                std::unique_ptr<ScratchImage> timage(new (std::nothrow) ScratchImage);
                if (!timage)
                {
                    RaiseMemoryAllocError(err_buf, err_buf_size);
                    return 1;
                }

                TexMetadata mdata = info;
                mdata.mipLevels = 1;
                hr = timage->Initialize(mdata);
                if (FAILED(hr))
                {
                    RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED [copy to single level] (", hr);
                    return 1;
                }

                if (info.dimension == TEX_DIMENSION_TEXTURE3D)
                {
                    for (size_t d = 0; d < info.depth; ++d)
                    {
                        hr = CopyRectangle(*image->GetImage(0, 0, d), Rect(0, 0, info.width, info.height),
                            *timage->GetImage(0, 0, d), TEX_FILTER_DEFAULT, 0, 0);
                        if (FAILED(hr))
                        {
                            RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED [copy to single level] (", hr);
                            return 1;
                        }
                    }
                }
                else
                {
                    for (size_t i = 0; i < info.arraySize; ++i)
                    {
                        hr = CopyRectangle(*image->GetImage(0, i, 0), Rect(0, 0, info.width, info.height),
                            *timage->GetImage(0, i, 0), TEX_FILTER_DEFAULT, 0, 0);
                        if (FAILED(hr))
                        {
                            RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED [copy to single level] (", hr);
                            return 1;
                        }
                    }
                }

                image.swap(timage);
                info.mipLevels = 1;
            }

            // --- Undo Premultiplied Alpha (if requested) ---------------------------------
            #if USE_ALPHA_CONFIG
            if ((dwOptions & (1 << OPT_DEMUL_ALPHA))
                && HasAlpha(info.format)
                && info.format != DXGI_FORMAT_A8_UNORM)
            {
                if (info.GetAlphaMode() == TEX_ALPHA_MODE_STRAIGHT)
                {
                    printf("\nWARNING: Image is already using straight alpha\n");
                }
                else if (!info.IsPMAlpha())
                {
                    printf("\nWARNING: Image is not using premultipled alpha\n");
                }
                else
                {
                    auto img = image->GetImage(0, 0, 0);
                    assert(img);
                    const size_t nimg = image->GetImageCount();

                    std::unique_ptr<ScratchImage> timage(new (std::nothrow) ScratchImage);
                    if (!timage)
                    {
                        RaiseMemoryAllocError(err_buf, err_buf_size);
                        return 1;
                    }

                    hr = PremultiplyAlpha(img, nimg, info, TEX_PMALPHA_REVERSE | dwSRGB, *timage);
                    if (FAILED(hr))
                    {
                        RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED [demultiply alpha] (", hr);
                        return 1;
                    }

                    auto& tinfo = timage->GetMetadata();
                    info.miscFlags2 = tinfo.miscFlags2;

                    assert(info.width == tinfo.width);
                    assert(info.height == tinfo.height);
                    assert(info.depth == tinfo.depth);
                    assert(info.arraySize == tinfo.arraySize);
                    assert(info.mipLevels == tinfo.mipLevels);
                    assert(info.miscFlags == tinfo.miscFlags);
                    assert(info.dimension == tinfo.dimension);

                    image.swap(timage);
                }
            }
            #endif

            // --- Resize ------------------------------------------------------------------
            if (!width)
            {
                width = info.width;
            }
            if (!height)
            {
                height = info.height;
            }
            if (info.width != width || info.height != height)
            {
                std::unique_ptr<ScratchImage> timage(new (std::nothrow) ScratchImage);
                if (!timage)
                {
                    RaiseMemoryAllocError(err_buf, err_buf_size);
                    return 1;
                }

                hr = Resize(image->GetImages(), image->GetImageCount(), image->GetMetadata(), width, height, dwFilter | dwFilterOpts, *timage.get());
                if (FAILED(hr))
                {
                    RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED [resize] (", hr);
                    return 1;
                }

                auto& tinfo = timage->GetMetadata();

                assert(tinfo.width == width && tinfo.height == height && tinfo.mipLevels == 1);
                info.width = tinfo.width;
                info.height = tinfo.height;
                info.mipLevels = 1;

                assert(info.depth == tinfo.depth);
                assert(info.arraySize == tinfo.arraySize);
                assert(info.miscFlags == tinfo.miscFlags);
                assert(info.format == tinfo.format);
                assert(info.dimension == tinfo.dimension);

                image.swap(timage);
            }
        
        #if USE_TONEMAP
            // --- Tonemap (if requested) --------------------------------------------------
            if (dwOptions & (1 << OPT_TONEMAP))
            {
                std::unique_ptr<ScratchImage> timage(new (std::nothrow) ScratchImage);
                if (!timage)
                {
                    RaiseMemoryAllocError(err_buf, err_buf_size);
                    return 1;
                }

                // Compute max luminosity across all images
                XMVECTOR maxLum = XMVectorZero();
                hr = EvaluateImage(image->GetImages(), image->GetImageCount(), image->GetMetadata(),
                    [&](const XMVECTOR* pixels, size_t w, size_t y)
                    {
                        UNREFERENCED_PARAMETER(y);

                        for (size_t j = 0; j < w; ++j)
                        {
                            static const XMVECTORF32 s_luminance = { { { 0.3f, 0.59f, 0.11f, 0.f } } };

                            XMVECTOR v = *pixels++;

                            v = XMVector3Dot(v, s_luminance);

                            maxLum = XMVectorMax(v, maxLum);
                        }
                    });
                if (FAILED(hr))
                {
                    RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED [tonemap maxlum] (", hr);
                    return 1;
                }

                // Reinhard et al, "Photographic Tone Reproduction for Digital Images"
                // http://www.cs.utah.edu/~reinhard/cdrom/
                maxLum = XMVectorMultiply(maxLum, maxLum);

                hr = TransformImage(image->GetImages(), image->GetImageCount(), image->GetMetadata(),
                    [&](XMVECTOR* outPixels, const XMVECTOR* inPixels, size_t w, size_t y)
                    {
                        UNREFERENCED_PARAMETER(y);

                        for (size_t j = 0; j < w; ++j)
                        {
                            XMVECTOR value = inPixels[j];

                            const XMVECTOR scale = XMVectorDivide(
                                XMVectorAdd(g_XMOne, XMVectorDivide(value, maxLum)),
                                XMVectorAdd(g_XMOne, value));
                            const XMVECTOR nvalue = XMVectorMultiply(value, scale);

                            value = XMVectorSelect(value, nvalue, g_XMSelect1110);

                            outPixels[j] = value;
                        }
                    }, *timage);
                if (FAILED(hr))
                {
                    RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED [tonemap apply] (", hr);
                    return 1;
                }

            #ifndef NDEBUG
                auto& tinfo = timage->GetMetadata();
            #endif

                assert(info.width == tinfo.width);
                assert(info.height == tinfo.height);
                assert(info.depth == tinfo.depth);
                assert(info.arraySize == tinfo.arraySize);
                assert(info.mipLevels == tinfo.mipLevels);
                assert(info.miscFlags == tinfo.miscFlags);
                assert(info.format == tinfo.format);
                assert(info.dimension == tinfo.dimension);

                image.swap(timage);
            }
        #endif

            // --- Convert -----------------------------------------------------------------
            if (format == DXGI_FORMAT_UNKNOWN)
            {
                format = info.format;
            }
            else if (info.format != format && !IsCompressed(format))
            {
                std::unique_ptr<ScratchImage> timage(new (std::nothrow) ScratchImage);
                if (!timage)
                {
                    RaiseMemoryAllocError(err_buf, err_buf_size);
                    return 1;
                }

                hr = Convert(image->GetImages(), image->GetImageCount(), image->GetMetadata(), format,
                    dwFilter | dwFilterOpts | dwSRGB, TEX_THRESHOLD_DEFAULT, *timage.get());
                if (FAILED(hr))
                {
                    RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED [convert] (", hr);
                    return 1;
                }

                auto& tinfo = timage->GetMetadata();

                assert(tinfo.format == format);
                info.format = tinfo.format;

                assert(info.width == tinfo.width);
                assert(info.height == tinfo.height);
                assert(info.depth == tinfo.depth);
                assert(info.arraySize == tinfo.arraySize);
                assert(info.mipLevels == tinfo.mipLevels);
                assert(info.miscFlags == tinfo.miscFlags);
                assert(info.dimension == tinfo.dimension);

                image.swap(timage);
            }

            images += info.arraySize;
            loadedImages.emplace_back(std::move(image));
        }
    }

    switch (dwCommand)
    {
    case CMD_CUBE:
        if (images != 6)
        {
            wprintf(L"\n");
            RaiseErrorMessage(err_buf, err_buf_size, L"ERROR: cube requires six images to form the faces of the cubemap\n");
            return 1;
        }
        break;

    case CMD_CUBEARRAY:
        if ((images < 6) || (images % 6) != 0)
        {
            RaiseErrorMessage(err_buf, err_buf_size, L"ERROR: cubearray requires a multiple of 6 images to form the faces of the cubemaps\n");
            return 1;
        }
        break;

    case CMD_H_CROSS:
    case CMD_V_CROSS:
    case CMD_H_STRIP:
    case CMD_V_STRIP:
#if USE_GIF
    case CMD_GIF:
#endif
        break;

    default:
        if (images < 2)
        {
            wprintf(L"\n");
            RaiseErrorMessage(err_buf, err_buf_size, L"ERROR: Need at least 2 images to assemble\n");
            return 1;
        }
        break;
    }

    // --- Create result ---------------------------------------------------------------
    switch (dwCommand)
    {
    case CMD_H_CROSS:
    case CMD_V_CROSS:
    case CMD_H_STRIP:
    case CMD_V_STRIP:
        {
            size_t twidth = 0;
            size_t theight = 0;

            switch (dwCommand)
            {
            case CMD_H_CROSS:
                //      posy
                // negx posz posx negz
                //      negy
                twidth = width * 4;
                theight = height * 3;
                break;

            case CMD_V_CROSS:
                //      posy
                // posz posx negz
                //      negy
                //      negx
                twidth = width * 3;
                theight = height * 4;
                break;

            case CMD_H_STRIP:
                twidth = width * 6;
                theight = height;
                break;

            case CMD_V_STRIP:
                twidth = width;
                theight = height * 6;
                break;

            default:
                break;
            }

            ScratchImage result;
            hr = result.Initialize2D(format, twidth, theight, 1, 1);
            if (FAILED(hr))
            {
                RaiseErrorCodeMessage(err_buf, err_buf_size, L"FAILED setting up result image (", hr);
                return 1;
            }

            memset(result.GetPixels(), 0, result.GetPixelsSize());

            auto src = loadedImages.cbegin();
            auto dest = result.GetImage(0, 0, 0);

            for (size_t index = 0; index < 6; ++index)
            {
                auto img = (*src)->GetImage(0, index, 0);
                if (!img)
                {
                    RaiseErrorMessage(err_buf, err_buf_size, L"FAILED: Unexpected error\n");
                    return 1;
                }

                const Rect rect(0, 0, width, height);

                size_t offsetx = 0;
                size_t offsety = 0;

                switch (dwCommand)
                {
                case CMD_H_CROSS:
                    {
                        //      posy
                        // negx posz posx negz
                        //      negy

                        static const size_t s_offsetx[6] = { 2, 0, 1, 1, 1, 3 };
                        static const size_t s_offsety[6] = { 1, 1, 0, 2, 1, 1 };

                        offsetx = s_offsetx[index] * width;
                        offsety = s_offsety[index] * height;
                        break;
                    }

                case CMD_V_CROSS:
                    {
                        //      posy
                        // negx posz posx
                        //      negy
                        //      negz

                        static const size_t s_offsetx[6] = { 2, 0, 1, 1, 1, 1 };
                        static const size_t s_offsety[6] = { 1, 1, 0, 2, 1, 3 };

                        offsetx = s_offsetx[index] * width;
                        offsety = s_offsety[index] * height;
                        break;
                    }

                case CMD_H_STRIP:
                    // posx, negx, posy, negy, posz, negz
                    offsetx = index * width;
                    break;

                case CMD_V_STRIP:
                    // posx, negx, posy, negy, posz, negz
                    offsety = index * height;
                    break;

                default:
                    break;
                }

                hr = CopyRectangle(*img, rect, *dest, dwFilter | dwFilterOpts, offsetx, offsety);
                if (FAILED(hr))
                {
                    RaiseErrorCodeMessage(err_buf, err_buf_size, L"FAILED building result image (", hr);
                    return 1;
                }
            }

            // Write cross/strip
            if (verbose){
                wprintf(L"\nWriting %ls ", szOutputFile);
                #if USE_PRINT_INFO
                PrintInfo(result.GetMetadata());
                #endif
                wprintf(L"\n");
            }
            fflush(stdout);

            #if USE_NAME_CONFIG
            if (dwOptions & (1 << OPT_TOLOWER))
            {
                std::ignore = _wcslwr_s(szOutputFile);
            }
            #endif

            if (~dwOptions & (1 << OPT_OVERWRITE))
            {
                #ifdef _WIN32
                if (GetFileAttributesW(szOutputFile) != INVALID_FILE_ATTRIBUTES)
                #else
                if (std::filesystem::exists(szOutputFile))
                #endif
                {
                    wprintf(L"\n");
                    RaiseErrorMessage(err_buf, err_buf_size, L"ERROR: Output file already exists, use -y to overwrite\n");
                    return 1;
                }
            }

            hr = SaveImageFile(*dest, fileType, szOutputFile);
            if (FAILED(hr))
            {
                RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED (", hr);
                return 1;
            }
            break;
        }

    case CMD_MERGE:
        {
            // Capture data from our second source image
            ScratchImage tempImage;
            hr = Convert(*loadedImages[1]->GetImage(0, 0, 0), DXGI_FORMAT_R32G32B32A32_FLOAT,
                dwFilter | dwFilterOpts | dwSRGB, TEX_THRESHOLD_DEFAULT, tempImage);
            if (FAILED(hr))
            {
                RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED [convert second input] (", hr);
                return 1;
            }

            const Image& img = *tempImage.GetImage(0, 0, 0);

            // Merge with our first source image
            const Image& rgb = *loadedImages[0]->GetImage(0, 0, 0);

            const XMVECTOR zc = XMVectorSelectControl(zeroElements[0], zeroElements[1], zeroElements[2], zeroElements[3]);
            const XMVECTOR oc = XMVectorSelectControl(oneElements[0], oneElements[1], oneElements[2], oneElements[3]);

            ScratchImage result;
            hr = TransformImage(rgb, [&, zc, oc](XMVECTOR* outPixels, const XMVECTOR* inPixels, size_t w, size_t y)
                {
                    const XMVECTOR *inPixels2 = reinterpret_cast<XMVECTOR*>(img.pixels + img.rowPitch * y);

                    for (size_t j = 0; j < w; ++j)
                    {
                        XMVECTOR pixel = XMVectorPermute(inPixels[j], inPixels2[j],
                            permuteElements[0], permuteElements[1], permuteElements[2], permuteElements[3]);
                        pixel = XMVectorSelect(pixel, g_XMZero, zc);
                        outPixels[j] = XMVectorSelect(pixel, g_XMOne, oc);
                    }
                }, result);
            if (FAILED(hr))
            {
                RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED [merge image] (", hr);
                return 1;
            }

            // Write merged texture
            if (verbose){
                wprintf(L"\nWriting %ls ", szOutputFile);
                #if USE_PRINT_INFO
                PrintInfo(result.GetMetadata());
                #endif
                wprintf(L"\n");
            }
            fflush(stdout);

            #if USE_NAME_CONFIG
            if (dwOptions & (1 << OPT_TOLOWER))
            {
                std::ignore = _wcslwr_s(szOutputFile);
            }
            #endif

            if (~dwOptions & (1 << OPT_OVERWRITE))
            {
                #ifdef _WIN32
                if (GetFileAttributesW(szOutputFile) != INVALID_FILE_ATTRIBUTES)
                #else
                if (std::filesystem::exists(szOutputFile))
                #endif
                {
                    wprintf(L"\n");
                    RaiseErrorMessage(err_buf, err_buf_size, L"ERROR: Output file already exists, use -y to overwrite\n");
                    return 1;
                }
            }

            hr = SaveImageFile(*result.GetImage(0, 0, 0), fileType, szOutputFile);
            if (FAILED(hr))
            {
                RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED (", hr);
                return 1;
            }
            break;
        }

    case CMD_ARRAY_STRIP:
        {
            const size_t twidth = width;
            const size_t theight = height * images;

            ScratchImage result;
            hr = result.Initialize2D(format, twidth, theight, 1, 1);
            if (FAILED(hr))
            {
                RaiseErrorCodeMessage(err_buf, err_buf_size, L"FAILED setting up result image (", hr);
                return 1;
            }

            memset(result.GetPixels(), 0, result.GetPixelsSize());

            auto src = loadedImages.cbegin();
            auto dest = result.GetImage(0, 0, 0);

            for (size_t index = 0; index < images; ++index)
            {
                auto img = (*src)->GetImage(0, index, 0);
                if (!img)
                {
                    RaiseErrorMessage(err_buf, err_buf_size, L"FAILED: Unexpected error\n");
                    return 1;
                }

                const Rect rect(0, 0, width, height);

                constexpr size_t offsetx = 0;
                size_t offsety = 0;

                offsety = index * height;

                hr = CopyRectangle(*img, rect, *dest, dwFilter | dwFilterOpts, offsetx, offsety);
                if (FAILED(hr))
                {
                    RaiseErrorCodeMessage(err_buf, err_buf_size, L"FAILED building result image (", hr);
                    return 1;
                }
            }

            // Write array strip
            if (verbose){
                wprintf(L"\nWriting %ls ", szOutputFile);
                #if USE_PRINT_INFO
                PrintInfo(result.GetMetadata());
                #endif
                wprintf(L"\n");
            }
            fflush(stdout);

            #if USE_NAME_CONFIG
            if (dwOptions & (1 << OPT_TOLOWER))
            {
                std::ignore = _wcslwr_s(szOutputFile);
            }
            #endif

            if (~dwOptions & (1 << OPT_OVERWRITE))
            {
                #ifdef _WIN32
                if (GetFileAttributesW(szOutputFile) != INVALID_FILE_ATTRIBUTES)
                #else
                if (std::filesystem::exists(szOutputFile))
                #endif
                {
                    wprintf(L"\n");
                    RaiseErrorMessage(err_buf, err_buf_size, L"ERROR: Output file already exists, use -y to overwrite\n");
                    return 1;
                }
            }

            hr = SaveImageFile(*dest, fileType, szOutputFile);
            if (FAILED(hr))
            {
                RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED (", hr);
                if (ErrorIsMissingPath(hr)){
                    RaiseErrorMessage(err_buf, err_buf_size, L"This error is mainly caused by missing the output directory.\n");
                }
                return 1;
            }
            break;
        }
    default:
        {
            std::vector<Image> imageArray;
            imageArray.reserve(images);

            for (auto it = loadedImages.cbegin(); it != loadedImages.cend(); ++it)
            {
                const ScratchImage* simage = it->get();
                assert(simage != nullptr);
                for (size_t j = 0; j < simage->GetMetadata().arraySize; ++j)
                {
                    const Image* img = simage->GetImage(0, j, 0);
                    assert(img != nullptr);
                    imageArray.push_back(*img);
                }
            }

            switch (dwCommand)
            {
            case CMD_CUBE:
                if (imageArray[0].width > maxCube || imageArray[0].height > maxCube)
                {
                    wprintf(L"\nWARNING: Target size exceeds maximum cube dimensions for feature level (%u)\n", maxCube);
                }
                break;

            case CMD_VOLUME:
                if (imageArray[0].width > maxVolume || imageArray[0].height > maxVolume || imageArray.size() > maxVolume)
                {
                    wprintf(L"\nWARNING: Target size exceeds volume extent for feature level (%u)\n", maxVolume);
                }
                break;

            case CMD_ARRAY:
                if (imageArray[0].width > maxSize || imageArray[0].height > maxSize || imageArray.size() > maxArray)
                {
                    wprintf(L"\nWARNING: Target size exceeds maximum size for feature level (size %u, array %u)\n", maxSize, maxArray);
                }
                break;

            case CMD_CUBEARRAY:
                if (imageArray[0].width > maxCube || imageArray[0].height > maxCube || imageArray.size() > maxArray)
                {
                    wprintf(L"\nWARNING: Target size exceeds maximum cube dimensions for feature level (size %u, array %u)\n", maxCube, maxArray);
                }
                break;

            default:
                if (imageArray[0].width > maxSize || imageArray[0].height > maxSize)
                {
                    wprintf(L"\nWARNING: Target size exceeds maximum size for feature level (%u)\n", maxSize);
                }
                break;
            }

            ScratchImage result;
            switch (dwCommand)
            {
            case CMD_VOLUME:
                hr = result.Initialize3DFromImages(&imageArray[0], imageArray.size());
                break;

            case CMD_ARRAY:
        #if USE_GIF
            case CMD_GIF:
        #endif
                hr = result.InitializeArrayFromImages(&imageArray[0], imageArray.size(), (dwOptions & (1 << OPT_USE_DX10)) != 0);
                break;

            case CMD_CUBE:
            case CMD_CUBEARRAY:
                hr = result.InitializeCubeFromImages(&imageArray[0], imageArray.size());
                break;

            default:
                break;
            }

            if (FAILED(hr))
            {
                wprintf(L"\n");
                RaiseErrorCodeMessage(err_buf, err_buf_size, L"FAILED building result image (", hr);
                return 1;
            }

            // Write texture
            if (verbose){
                wprintf(L"\nWriting %ls ", szOutputFile);
                #if USE_PRINT_INFO
                PrintInfo(result.GetMetadata());
                #endif
                wprintf(L"\n");
            }
            fflush(stdout);

            #if USE_NAME_CONFIG
            if (dwOptions & (1 << OPT_TOLOWER))
            {
                std::ignore = _wcslwr_s(szOutputFile);
            }
            #endif

            if (~dwOptions & (1 << OPT_OVERWRITE))
            {
                #ifdef _WIN32
                if (GetFileAttributesW(szOutputFile) != INVALID_FILE_ATTRIBUTES)
                #else
                if (std::filesystem::exists(szOutputFile))
                #endif
                {
                    wprintf(L"\n");
                    RaiseErrorMessage(err_buf, err_buf_size, L"ERROR: Output file already exists, use -y to overwrite\n");
                    return 1;
                }
            }

            hr = SaveToDDSFile(result.GetImages(), result.GetImageCount(), result.GetMetadata(),
                (dwOptions & (1 << OPT_USE_DX10)) ? (DDS_FLAGS_FORCE_DX10_EXT | DDS_FLAGS_FORCE_DX10_EXT_MISC2) : DDS_FLAGS_NONE,
                szOutputFile);
            if (FAILED(hr))
            {
                RaiseErrorCodeMessage(err_buf, err_buf_size, L" FAILED (", hr);
                if (ErrorIsMissingPath(hr)){
                    RaiseErrorMessage(err_buf, err_buf_size, L"This error is mainly caused by missing the output directory.\n");
                }
                return 1;
            }
            break;
        }
    }

    return 0;
}

// Main function for exe
#if BUILD_AS_EXE
#ifdef _WIN32
int __cdecl wmain(_In_ int argc, _In_z_count_(argc) wchar_t* argv[])
{
    bool verbose = true;
    bool init_com = true;
#else
int main(_In_ int argc, _In_z_count_(argc) char* argv_char[])
{
    bool verbose = true;
    bool init_com = false;

    wchar_t* argv[argc];
    size_t length;
    for(int i=0;i<argc;i++){
        length = strlen(argv_char[i]);
        argv[i] = new wchar_t[length + 1];
        mbstowcs(argv[i], argv_char[i], length);
    }

#endif  // _WIN32
    if (argc == 0){
        return texassemble(0, argv, verbose, init_com);
    } else {
        return texassemble(argc - 1, &argv[1], verbose, init_com);
    }
}
#endif  // BUILD_AS_EXE