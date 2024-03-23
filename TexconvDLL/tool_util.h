#pragma once

#include "TexconvDLL/config.h"
#include <cstdio>
#include <wrl/client.h>

#ifndef _WIN32
extern "C"{
#include <safe_str_lib.h>
}
#include <filesystem>

//#define S_OK 0
//#define E_FAIL 0x80004005
//#define FAILED(hr) hr != S_OK

// Todo: Use secure function
#define swscanf_s swscanf
#define swprintf_s swprintf

// wcsicmp for Windows = wcscasecmp for Unix
#define _wcsicmp wcscasecmp

// undefined parameters for unix
#define _MAX_PATH 260
#define _MAX_EXT 256
#define _MAX_FNAME 256

// Todo: define _wcslwr_s for unix
// (for USE_NAME_CONFIG)

// wmakepath_s for unix
template <size_t size>
void _wmakepath_s(
   wchar_t (&path)[size],
   const wchar_t *drv,
   const wchar_t *dir,
   const wchar_t *fname,
   const wchar_t *ext
) {
    wchar_t* end = &path[0] + size;    /* end of path buffer */
    wchar_t* s = &path[0];      /* copy pointer */
    if (drv) {
        for(;*drv && *drv != '\0' && s < end;)
            *s++ = *drv++;
    }
    if (dir) {
        for(;*dir && *dir!= '\0' && s < end;)
            *s++ = *dir++;
    }
    if (s > &path[0] && s < end && (*(s - 1) != std::filesystem::path::preferred_separator)) {
        *s++ = std::filesystem::path::preferred_separator;
    }
    if (fname) {
        for(; *fname && *fname!= '\0' && s < end;)
            *s++ = *fname++;
    }
    if (s < end) {
        *s++ = '.';
    }
    if (ext) {
        for(; *ext && *ext!= '\0' && s < end;)
            *s++ = *ext++;
    }
    if (s < end) {
        *s = '\0';
    }
}

// wsplitpath_s for unix
static void _wsplitpath_s(const WCHAR* path, WCHAR* drv, int drvnum, WCHAR* dir, int dirnum, WCHAR* name, int namenum, WCHAR* ext, int extnum)
{
    const WCHAR* end; /* end of processed string */
    const WCHAR* p;      /* search pointer */
    const WCHAR* s;      /* copy pointer */

    /* extract drive name */
    if (path[0] && path[1]==':') {
            if (drv) {
                    *drv++ = *path++;
                    *drv++ = *path++;
                    *drv = '\0';
            }
    } else if (drv)
            *drv = '\0';

    /* search for end of string or stream separator */
    for(end=path; *end && *end!=':'; )
            end++;


    /* search for begin of file extension */
    for(p=end; p>path && *--p!='\\' && *p!='/'; )
            if (*p == '.') {
                    end = p;
                    break;
            }

    if (ext)
            for(s=end; (*ext=*s++); )
                    ext++;

    /* search for end of directory name */
    for(p=end; p>path; )
            if (*--p=='\\' || *p=='/') {
                    p++;
                    break;
            }

    if (name) {
            for(s=p; s<end; )
                    *name++ = *s++;

            *name = '\0';
    }

    if (dir) {
            for(s=path; s<p; )
                    *dir++ = *s++;

            *dir = '\0';
    }
}
#endif

#ifdef _WIN32
#define ErrorIsMissingPath(hr) hr == -2147024893;
#else
#define ErrorIsMissingPath(hr) hr == -2147467259;
#endif

#define RaiseError(...) \
    fwprintf(stderr, __VA_ARGS__); \
    if (err_buf != nullptr) { \
        swprintf(err_buf, err_buf_size, __VA_ARGS__); \
    }

#define PrintInfoVerbose(...) \
    if (verbose) PrintInfo(__VA_ARGS__)

#define PrintVerbose(...) \
    if (verbose) wprintf(__VA_ARGS__)
