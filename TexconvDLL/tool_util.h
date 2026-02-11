#pragma once

#include "config.h"
#include <cstdio>
#include <wrl/client.h>

#ifndef _WIN32
extern "C"{
#include <safe_str_lib.h>
}
#include <filesystem>

// Todo: Use secure function
#define swscanf_s swscanf
#define swprintf_s swprintf

// wcsicmp for Windows = wcscasecmp for Unix
#define _wcsicmp wcscasecmp

// TODO: replace MAX_PATH with PATH_MAX for linux?
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

// Todo: define _wcslwr_s for unix
// (for USE_NAME_CONFIG)

#endif  // _WIN32

#define RaiseError(...) \
    fwprintf(stderr, __VA_ARGS__); \
    if (err_buf != nullptr) { \
        swprintf(err_buf, err_buf_size, __VA_ARGS__); \
    }

#define PrintInfoVerbose(...) \
    if (verbose) PrintInfo(__VA_ARGS__)

#define PrintVerbose(...) \
    if (verbose) wprintf(__VA_ARGS__)
