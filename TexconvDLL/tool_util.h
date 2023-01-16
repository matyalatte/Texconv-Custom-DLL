#pragma once

#include <cstdio>
#include <wrl/client.h>

// Define slash for paths
#ifdef _WIN32
const wchar_t WSLASH = L'\\';
const char SLASH = '\\';
#else
const wchar_t WSLASH = L'/';
const char SLASH = '/';
#endif

#ifndef _WIN32
extern "C"{
#include <safe_str_lib.h>
}
#include <filesystem>

// Todo: Use secure function
#define swscanf_s swscanf

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
    if (s > &path[0] && s < end && (*(s - 1) != SLASH)) {
        *s++ = SLASH;
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
void _wsplitpath_s(const WCHAR* path, WCHAR* drv, int drvnum, WCHAR* dir, int dirnum, WCHAR* name, int namenum, WCHAR* ext, int extnum);
#endif

const wchar_t* GetErrorDesc(HRESULT hr);

bool ErrorIsMissingPath(HRESULT hr);

void RaiseErrorMessage(wchar_t* err_buf, size_t err_buf_size, const wchar_t* msg);

void RaiseErrorMessage(wchar_t* err_buf, size_t err_buf_size, int num);

void RaiseErrorMessage(wchar_t* err_buf, size_t err_buf_size, const wchar_t* msg, const wchar_t* msg2, const wchar_t* msg3);

void RaiseMemoryAllocError(wchar_t* err_buf, size_t err_buf_size);

void RaiseErrorCodeMessage(wchar_t* err_buf, size_t err_buf_size, const wchar_t* msg, HRESULT hr);

void RaiseInvalidOptionError(wchar_t* err_buf, size_t err_buf_size, const wchar_t* option, const wchar_t* pValue);

void RaiseInvalidOptionError(wchar_t* err_buf, size_t err_buf_size, const wchar_t* msg);

void RaiseInvalidOptionError(wchar_t* err_buf, size_t err_buf_size, const wchar_t* msg, const wchar_t* msg2, const wchar_t* msg3);
