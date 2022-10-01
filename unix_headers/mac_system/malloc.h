#pragma once
#ifdef __APPLE__
#include <stdlib.h>
inline void *aligned_alloc(size_t alignment, size_t size){
    void* memptr;
    int err = posix_memalign(&memptr, alignment, size);
    if (err) {
        memptr = nullptr;
    }
    return memptr;

}
#endif