#pragma once
#include <cwchar>
#include <comdef.h>

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

// Todo: define _wcslwr_s and _wmakepath_s for unix
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