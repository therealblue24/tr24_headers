/* tr24_valid_ptr.h - v0.01 - public domain therealblue24 2023
 * This header has a function which detects if a given pointer is pointing to a valid heap object.
 * Note that this DOES NOT check if the pointer is freed. It only checks if it's valid.
 *
 * C isn't safe, you should have known that by now.
 *
 * This file provides both the interface and the implementation.
 * To init the implementation,
 *      #define TR24_VALID_PTR_IMPL or
 *      #define TR24_IMPL
 * in *one* source file, before #including to generate the implementation.
 *
 * Examples are in examples folder.
 *
 * History:
 *      0.01 first public release
 */
#ifndef TR24_VALID_PTR_H_
#define TR24_VALID_PTR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

#ifndef TR24_ALWAYS_INLINE
#define TR24_ALWAYS_INLINE __attribute__((always_inline)) inline
#endif /* TR24_ALWAYS_INLINE */

bool tr24_is_valid(const void *ptr, size_t bytes);
TR24_ALWAYS_INLINE bool tr24_isnot_valid(const void *ptr, size_t bytes);

#ifdef __cplusplus
}
#endif

#endif /* TR24_VALID_PTR_H_ */

#ifdef TR24_IMPL
#define TR24_VALID_PTR_IMPL
#endif /* TR24_IMPL */

#ifdef TR24_VALID_PTR_IMPL
#undef TR24_VALID_PTR_IMPL

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <errno.h>

typedef struct {
    int fd[2];
    bool ran_ctor, ran_dtor;
} _tr24_valid_ptr_internal;
static _tr24_valid_ptr_internal _vpi;

bool tr24_is_valid(const void *ptr, size_t bytes)
{
    if(ptr == NULL) {
        return false;
    }
    bool ret = true;
    if(write(_vpi.fd[1], ptr, bytes) < 0) {
        if(errno == EFAULT) {
            ret = false;
        }
    }
    return ret;
}

TR24_ALWAYS_INLINE bool tr24_isnot_valid(const void *ptr, size_t bytes)
{
    return !tr24_is_valid(ptr, bytes);
}

__attribute__((constructor)) void _tr24_ctor_init()
{
    if(_vpi.ran_ctor) {
    } else {
        pipe(_vpi.fd);
        _vpi.ran_ctor = true;
    }
}

__attribute__((destructor)) void _tr24_dtor_deinit()
{
    if(_vpi.ran_dtor) {
    } else {
        close(_vpi.fd[0]);
        close(_vpi.fd[1]);
        _vpi.ran_dtor = true;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* TR24_VALID_PTR_IMPL */

/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/
