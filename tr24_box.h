/* tr24_box.h - v0.01 - public domain therealblue24 2023
 * Pointer tagged with size that can do some stuff. Pretty simple.
 * 
 * This file provides both the interface and the implementation.
 * To init the implementation,
 *      #define TR24_BOX_IMPL or
 *      #define TR24_IMPL
 * in *one* source file, before #including to generate the implementation.
 *
 * No real need for examples, the box is a pointer encapsulator.
 *
 * History:
 *      0.01 first public release
 */
#ifndef TR24_BOX_H_
#define TR24_BOX_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TR24_MEMCPY
#define TR24_MEMCPY memcpy
#endif /* TR24_MEMCPY */

#ifndef TR24_ASSERT
#define TR24_ASSERT assert
#endif /* TR24_ASSERT */

#ifndef TR24_MALLOC
#define TR24_MALLOC malloc
#endif /* TR24_MALLOC */

#ifndef TR24_ALIGNED_ALLOC
#define TR24_ALIGNED_ALLOC aligned_alloc
#endif /* TR24_ALIGNED_ALLOC */

#ifndef TR24_FREE
#define TR24_FREE free
#endif /* TR24_FREE */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Would you seriously allocate more than 4GB of mem for one single pointer? */
typedef struct _tr24_box_layout {
    uint32_t size;
    int32_t align;
} __attribute__((packed)) _tr24_box_layout;

/* Takes 2 machine words. Not that much overhead. */
typedef struct tr24_box {
    _tr24_box_layout layout;
    void *ptr;
} __attribute__((packed)) tr24_box_t;

tr24_box_t tr24_box_create(void *ptr, size_t size, int32_t align,
                           void *(*ctor)(size_t size, int32_t align));
tr24_box_t tr24_box_copy(tr24_box_t box);
tr24_box_t tr24_box_clone(tr24_box_t box);
int tr24_box_delete(tr24_box_t box, void (*dtor)(tr24_box_t box));

#ifdef __cplusplus
}
#endif

#endif /* TR24_BOX_H_ */

#ifdef TR24_IMPL
#define TR24_BOX_IMPL
#endif /* TR24_IMPL */

#ifdef TR24_BOX_IMPL
#undef TR24_BOX_IMPL

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

tr24_box_t tr24_box_create(void *ptr, size_t size, int32_t align,
                           void *(*ctor)(size_t size, int32_t align))
{
    tr24_box_t ret = (tr24_box_t){
        .layout = (_tr24_box_layout){ .size = (uint32_t)size, .align = align },
        .ptr = ptr,
    };
    if(ctor && ptr == NULL) {
        ret.ptr = ctor(size, align);
    }
    return ret;
}

tr24_box_t tr24_box_copy(tr24_box_t box)
{
    return tr24_box_create(box.ptr, (uint32_t)box.layout.size, box.layout.align,
                           NULL);
}

tr24_box_t tr24_box_clone(tr24_box_t box)
{
    if(!box.ptr) {
        if(box.layout.align < 2) {
            return tr24_box_create(TR24_MALLOC(box.layout.size),
                                   box.layout.size, -1, NULL);
        } else {
            return tr24_box_create(TR24_ALIGNED_ALLOC(box.layout.size,
                                                      box.layout.align),
                                   box.layout.size, -1, NULL);
        }
    }

    void *newboxptr;
    if(box.layout.align < 2) {
        newboxptr = TR24_MALLOC(box.layout.size);
    } else {
        newboxptr = TR24_ALIGNED_ALLOC(box.layout.align, box.layout.size);
    }
    TR24_MEMCPY(newboxptr, box.ptr, box.layout.size);
    return tr24_box_create(newboxptr, box.layout.size, box.layout.align, NULL);
}

int tr24_box_delete(tr24_box_t box, void (*dtor)(tr24_box_t box))
{
    if(box.ptr) {
        if(dtor)
            dtor(box);
        TR24_FREE(box.ptr);
    }
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* TR24_BOX_IMPL */

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
