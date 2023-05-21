/* tr24_mutex.h - v0.01 - public domain therealblue24 2023
 * Simple Mutex Implementation for C
 * 
 * This file provides both the interface and the implementation.
 * To init the implementation,
 *      #define TR24_MUTEX_IMPL or
 *      #define TR24_IMPL
 * in *one* source file, before #including to generate the implementation.
 *
 * Examples are in examples folder.
 *
 * History:
 *      0.01 first public release
 */
#ifndef TR24_MUTEX_H_
#define TR24_MUTEX_H_

#include <stdbool.h>

struct tr24_mutex_t {
    void *value;
    bool locked;
    bool in_session_locked;
};
typedef struct tr24_mutex_t tr24_mutex_t;

tr24_mutex_t tr24_mutex_create(void *val);
void tr24_mutex_lock(tr24_mutex_t mtx);
void tr24_mutex_unlock(tr24_mutex_t mtx);
void *tr24_mutex_get(tr24_mutex_t mtx);

#define tr24_mutex_set(mtx, var, val)          \
    while(mtx.locked || mtx.in_session_locked) \
        ;                                      \
    tr24_mutex_lock(mtx);                      \
    var = val;                                 \
    tr24_mutex_unlock(mtx);

#endif /* TR24_MUTEX_H_ */

#ifdef TR24_IMPL
#define TR24_MUTEX_IMPL
#endif /* TR24_IMPL */

#ifdef TR24_MUTEX_IMPL
#undef TR24_MUTEX_IMPL

tr24_mutex_t tr24_mutex_create(void *val)
{
    tr24_mutex_t ret = { .value = val, false, false };
    return ret;
}

void tr24_mutex_lock(tr24_mutex_t mtx)
{
    while(mtx.locked || mtx.in_session_locked)
        ;
    mtx.in_session_locked = true;
    mtx.locked = true;
    mtx.in_session_locked = false;
}

void tr24_mutex_unlock(tr24_mutex_t mtx)
{
    mtx.in_session_locked = true;
    mtx.locked = false;
    mtx.in_session_locked = false;
}

void *tr24_mutex_get(tr24_mutex_t mtx)
{
    return mtx.value;
}

#endif /* TR24_MUTEX_IMPL */

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
