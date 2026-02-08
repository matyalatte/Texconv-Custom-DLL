#include "DirectXTex.h"
#include "DirectXPackedVector.h"
using namespace DirectX;

#ifdef _WIN32
extern "C" __declspec(dllexport) int __cdecl texconv(
#else
extern "C" __attribute__((visibility("default"))) int texconv(
#endif
    int argc, wchar_t* argv[],
    bool verbose, bool init_com,
    bool allow_slow_codec,
    wchar_t* err_buf, int err_buf_size);
