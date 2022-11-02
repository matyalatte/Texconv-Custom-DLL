#include "tool_util.h"

#ifndef _WIN32
void _wsplitpath_s(const WCHAR* path, WCHAR* drv, int drvnum, WCHAR* dir, int dirnum, WCHAR* name, int namenum, WCHAR* ext, int extnum)
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

const wchar_t* GetErrorDesc(HRESULT hr)
{
#ifdef _WIN32
    static wchar_t desc[1024] = {};
    LPWSTR errorText = nullptr;

    const DWORD result = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        nullptr, static_cast<DWORD>(hr),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&errorText), 0, nullptr);

    *desc = 0;

    if (result > 0 && errorText)
    {
        swprintf_s(desc, L": %ls", errorText);

        size_t len = wcslen(desc);
        if (len >= 1)
        {
            desc[len - 1] = 0;
        }

        if (errorText)
            LocalFree(errorText);
    }
    return desc;
#else
    return L"";
#endif
}

bool ErrorIsMissingPath(HRESULT hr){
#ifdef _WIN32
    return hr == -2147024893;
#else
    return hr == -2147467259;
#endif
}

void RaiseErrorMessage(wchar_t* err_buf, size_t err_buf_size, const wchar_t* msg) {
    fwprintf(stderr, L"%ls", msg);
    if (err_buf != nullptr) {
        wcscat_s(err_buf, err_buf_size, msg);
    }
}

void RaiseErrorMessage(wchar_t* err_buf, size_t err_buf_size, int num) {
    wchar_t buf[128] = {};
    swprintf(buf, 128, L"%d", num);
    RaiseErrorMessage(err_buf, err_buf_size, buf);
}

void RaiseErrorMessage(wchar_t* err_buf, size_t err_buf_size, const wchar_t* msg, const wchar_t* msg2, const wchar_t* msg3) {
    wchar_t buf[512] = {};
    swprintf(buf, 512, L"%ls%ls%ls", msg, msg2, msg3);
    RaiseErrorMessage(err_buf, err_buf_size, buf);
}

void RaiseMemoryAllocError(wchar_t* err_buf, size_t err_buf_size) {
    wprintf(L"\n");
    RaiseErrorMessage(err_buf, err_buf_size, L"ERROR: Memory allocation failed\n");
}

void RaiseErrorCodeMessage(wchar_t* err_buf, size_t err_buf_size, const wchar_t* msg, HRESULT hr) {
    wchar_t buf[512] = {};
    swprintf(buf, 512, L"%ls%08X%ls)\n", msg, static_cast<int>(hr), GetErrorDesc(hr));
    RaiseErrorMessage(err_buf, err_buf_size, buf);
}

void RaiseInvalidOptionError(wchar_t* err_buf, size_t err_buf_size, const wchar_t* option, const wchar_t* pValue) {
    wchar_t buf[512] = {};
    swprintf(buf, 512, L"Invalid value specified with %ls (%ls)\n", option, pValue);
    RaiseErrorMessage(err_buf, err_buf_size, buf);
#if USE_USAGE
    wprintf(L"\n");
    PrintUsage();
#endif
}

void RaiseInvalidOptionError(wchar_t* err_buf, size_t err_buf_size, const wchar_t* msg) {
    RaiseErrorMessage(err_buf, err_buf_size, msg);
#if USE_USAGE
    wprintf(L"\n");
    PrintUsage();
#endif
}

void RaiseInvalidOptionError(wchar_t* err_buf, size_t err_buf_size, const wchar_t* msg, const wchar_t* msg2, const wchar_t* msg3) {
    RaiseErrorMessage(err_buf, err_buf_size, msg, msg2, msg3);
#if USE_USAGE
    wprintf(L"\n");
    PrintUsage();
#endif
}