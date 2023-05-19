/* tr24_smartptr.h - v0.02 - public domain therealblue24 2023
 * A C smart pointer library using hacky GNU C extensions.
 * 
 * This file provides both the interface and the implementation.
 * To init the implementation,
 *      #define TR24_SMARTPTR_IMPL
 * in *one* source file, before #including to generate the implementation.
 *
 * Examples are found in the examples folder.
 *
 * There are a few macros you need to know about:
 *       - tr24_smart
 *       - tr24_unique_ptr
 *       - tr24_shared_ptr
 * tr24_smart is the attribute, which is important. The other two are the
 * main spotlight. Example:
 *
 * int main() {
 *      tr24_smart int *ptr = tr24_unique_ptr(int, 42);
 *      // pointer is now 42
 * }
 * 
 * All pointers are autofreed, such the "smart pointer" title.
 *
 * You can also specify a custom destructor:
 *
 * void cleanup(void *ptr, void *meta) {
 *      (void)meta;
 *      printf("haha ptr is %p, got called destructor\n", ptr);
 * }
 *
 * int main() {
 *      tr24_smart int *ptr = tr24_unique_ptr(int, 42, cleanup);
 *      // cleanup = destructor, val = 42
 *      return 0;
 *      // destructor is called
 * }
 *
 * (Destructors do not free the base pointer.)
 * More examples are in examples folder.
 * Requires a C compiler with capable GNU C extensions (for both C23 & below).
 * Happy Hacking!
 * -therealblue24
 *
 * History:
 *      0.02 first public release
 *      0.01 replace malloc, free, realloc and more calls with macros
 *      0.00 it works.
 */

#ifndef TR24_SMARTPTR_H_
#define TR24_SMARTPTR_H_

/* We want to use C23 attributes when we can */
#if __STDC_VERSION__ == 202311L
#ifndef TR24_INLINE
#define TR24_INLINE [[gnu::always_inline]] inline
#endif /* TR24_INLINE */
#ifndef TR24_MALLOC_API
#define TR24_MALLOC_API [[gnu::malloc]]
#endif /* TR24_MALLOC_API */
#ifndef TR24_PURE
#define TR24_PURE [[gnu::pure]]
#endif /* TR24_PURE */
#else
#ifndef TR24_INLINE
#define TR24_INLINE __attribute__((always_inline)) inline
#endif /* TR24_INLINE */
#ifndef TR24_MALLOC_API
#define TR24_MALLOC_API __attribute__((malloc))
#endif /* TR24_MALLOC_API */
#ifndef TR24_PURE
#define TR24_PURE __attribute__((pure))
#endif /* TR24_PURE */
#endif /* __STDC_VERSION__ */

#include <stdlib.h>

#define TR24SP_SENTINEL .sentinel_ = 0,
#define TR24SP_SENTINEL_DEC int sentinel_;

enum tr24sp__pointer {
    TR24SP__UNIQUE,
    TR24SP__SHARED,

    TR24SP__ARRAY = 1 << 8
};

typedef void (*tr24sp__f_destruct)(void *, void *);

typedef struct {
    void *(*alloc)(size_t);
    void (*dealloc)(void *);
} tr24sp__s_allocator;

extern tr24sp__s_allocator tr24sp__smalloc_allocator;

typedef struct {
    TR24SP_SENTINEL_DEC
    size_t size;
    size_t nmemb;
    enum tr24sp__pointer kind;
    tr24sp__f_destruct dtor;
    struct {
        const void *data;
        size_t size;
    } meta;
} tr24sp__s_smalloc_args;

TR24_PURE void *tr24sp__get_smart_ptr_meta(void *ptr);
void *tr24sp__sref(void *ptr);
#define tr24_sref tr24sp__sref
TR24_MALLOC_API void *tr24sp__smalloc(tr24sp__s_smalloc_args *args);
void *tr24sp__srealloc(size_t type, void *ptr, size_t size);
void tr24sp__sfree(void *ptr);
void *tr24sp__smove_size(void *ptr, size_t size);

#define tr24sp__smalloc_m(...) \
    tr24sp__smalloc(&(tr24sp__s_smalloc_args){ TR24SP_SENTINEL __VA_ARGS__ })

#define tr24_smalloc tr24sp__smalloc_m

#define tr24sp__smove(p) tr24sp__smove_size((p), sizeof(*(p)))

#include <string.h>

TR24_INLINE void tr24sp__sfree_stack(void *ptr)
{
    union {
        void **real_ptr;
        void *ptr;
    } conv;
    conv.ptr = ptr;
    tr24sp__sfree(*conv.real_ptr);
    *conv.real_ptr = NULL;
}

// clang-format off
#define TR24SP__ARGS_ args.dtor, { args.meta.ptr, args.meta.size }
// clang-format on

#if __STDC_VERSION__ == 202311L
#define tr24_smart [[gnu::cleanup(tr24sp__sfree_stack)]]
#else
#define tr24_smart __attribute__((cleanup(tr24sp__sfree_stack)))
#endif /* __STDC_VERSION__ */

#ifndef TR24_MEMCPY
#define TR24_MEMCPY memcpy
#endif

#define tr24__smart_ptr(k, t, ...)                                             \
    ({                                                                         \
        struct s_tmp {                                                         \
            TR24SP_SENTINEL_DEC                                                \
            __typeof__(t) value;                                               \
            tr24sp__f_destruct dtor;                                           \
            struct {                                                           \
                const void *ptr;                                               \
                size_t size;                                                   \
            } meta;                                                            \
        } args = { TR24SP_SENTINEL __VA_ARGS__ };                              \
        const __typeof__(t[1]) dummy;                                          \
        void *var = sizeof(dummy[0]) == sizeof(dummy) ?                        \
                        tr24sp__smalloc_m(sizeof(t), 0, k, TR24SP__ARGS_) :    \
                        tr24sp__smalloc_m(sizeof(dummy[0]),                    \
                                          sizeof(dummy) / sizeof(dummy[0]), k, \
                                          TR24SP__ARGS_);                      \
        if(var != NULL)                                                        \
            TR24_MEMCPY(var, &args.value, sizeof(t));                          \
        var;                                                                   \
    })

#define tr24__smart_arr(k, t, l, ...)                                  \
    ({                                                                 \
        struct s_tmp {                                                 \
            TR24SP_SENTINEL_DEC                                        \
            __typeof__(__typeof__(t)[l]) value;                        \
            tr24sp__f_destruct dtor;                                   \
            struct {                                                   \
                const void *ptr;                                       \
                size_t size;                                           \
            } meta;                                                    \
        } args = { TR24SP_SENTINEL __VA_ARGS__ };                      \
        void *var = tr24sp__smalloc_m(sizeof(t), l, k, TR24SP__ARGS_); \
        if(var != NULL)                                                \
            TR24_MEMCPY(var, &args.value, sizeof(t));                  \
        var;                                                           \
    })

#define tr24_shared_ptr(t, ...) tr24__smart_ptr(TR24SP__SHARED, t, __VA_ARGS__)
#define tr24_unique_ptr(t, ...) tr24__smart_ptr(TR24SP__UNIQUE, t, __VA_ARGS__)

#define tr24_shared_arr(t, l, ...) \
    tr24__smart_arr(TR24SP__SHARED, t, l, __VA_ARGS__)
#define tr24_unique_arr(t, l, ...) \
    tr24__smart_arr(TR24SP__UNIQUE, t, l, __VA_ARGS__)

typedef struct {
    size_t nmemb;
    size_t size;
} tr24sp__s_meta_array;

TR24_PURE size_t tr24sp__array_length(void *ptr);
#define array_length tr24sp__array_length

TR24_PURE size_t tr24sp__array_type_size(void *ptr);
#define array_type_size tr24sp__array_type_size

TR24_PURE void *tr24sp__array_user_meta(void *ptr);
#define array_user_meta tr24sp__array_user_meta

#endif /* TR24_SMARTPTR_H_ */

#ifdef TR24_SMARTPTR_IMPL
#undef TR24_SMARTPTR_IMPL

TR24_PURE size_t tr24sp__array_length(void *ptr)
{
    tr24sp__s_meta_array *meta = tr24sp__get_smart_ptr_meta(ptr);
    return meta ? meta->nmemb : 0;
}

TR24_PURE size_t tr24sp__array_type_size(void *ptr)
{
    tr24sp__s_meta_array *meta = tr24sp__get_smart_ptr_meta(ptr);
    return meta ? meta->size : 0;
}

TR24_PURE TR24_INLINE void *tr24sp__array_user_meta(void *ptr)
{
    tr24sp__s_meta_array *meta = tr24sp__get_smart_ptr_meta(ptr);
    return meta ? meta + 1 : NULL;
}

typedef struct {
    enum tr24sp__pointer kind;
    tr24sp__f_destruct dtor;
#ifndef NDEBUG
    void *ptr;
#endif /* !NDEBUG */
} tr24sp__s_meta;

typedef struct {
    enum tr24sp__pointer kind;
    tr24sp__f_destruct dtor;
#ifndef NDEBUG
    void *ptr;
#endif /* !NDEBUG */
    volatile size_t ref_count;
} tr24sp__s_meta_shared;

TR24_INLINE size_t tr24sp__align(size_t s)
{
    return (s + (sizeof(char *) - 1)) & ~(sizeof(char *) - 1);
}

TR24_PURE TR24_INLINE tr24sp__s_meta *tr24sp__get_meta(void *ptr)
{
    size_t *size = (size_t *)ptr - 1;
    return (tr24sp__s_meta *)((char *)size - *size);
}

#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#undef tr24sp__smalloc

#ifndef TR24_ASSERT
#define TR24_ASSERT assert
#endif /* TR24_ASSERT */

#ifndef TR24_MALLOC
#define TR24_MALLOC malloc
#endif /* TR24_MALLOC */

#ifndef TR24_FREE
#define TR24_FREE free
#endif /* TR24_FREE */

tr24sp__s_allocator tr24sp__smalloc_allocator = { TR24_MALLOC, TR24_FREE };

TR24_INLINE static size_t atomic_add(volatile size_t *count, const size_t limit,
                                     const size_t val)
{
    size_t old_count, new_count;
    do {
        old_count = *count;
        if(old_count == limit)
            abort();
        new_count = old_count + val;
    } while(!__sync_bool_compare_and_swap(count, old_count, new_count));
    return new_count;
}

TR24_INLINE static size_t atomic_increment(volatile size_t *count)
{
    return atomic_add(count, SIZE_MAX, 1);
}

TR24_INLINE static size_t atomic_decrement(volatile size_t *count)
{
    return atomic_add(count, 0, -1);
}

TR24_INLINE void *tr24sp__get_smart_ptr_meta(void *ptr)
{
    TR24_ASSERT((size_t)ptr == tr24sp__align((size_t)ptr));

    tr24sp__s_meta *meta = tr24sp__get_meta(ptr);
    TR24_ASSERT(meta->ptr == ptr);

    size_t head_size = meta->kind & TR24SP__SHARED ?
                           sizeof(tr24sp__s_meta_shared) :
                           sizeof(tr24sp__s_meta);
    size_t *metasize = (size_t *)ptr - 1;
    if(*metasize == head_size)
        return NULL;

    return (char *)meta + head_size;
}

void *tr24sp__sref(void *ptr)
{
    tr24sp__s_meta *meta = tr24sp__get_meta(ptr);
    TR24_ASSERT(meta->ptr == ptr);
    TR24_ASSERT(meta->kind & TR24SP__SHARED);
    atomic_increment(&((tr24sp__s_meta_shared *)meta)->ref_count);
    return ptr;
}

void *tr24sp__smove_size(void *ptr, size_t size)
{
    tr24sp__s_meta *meta = tr24sp__get_meta(ptr);
    TR24_ASSERT(meta->kind & TR24SP__UNIQUE);

    tr24sp__s_smalloc_args args;

    size_t *metasize = (size_t *)ptr - 1;
    if(meta->kind & TR24SP__ARRAY) {
        tr24sp__s_meta_array *arr_meta = tr24sp__get_smart_ptr_meta(ptr);
        args = (tr24sp__s_smalloc_args){
            .size = arr_meta->size * arr_meta->nmemb,
            .kind = (enum tr24sp__pointer)(TR24SP__SHARED | TR24SP__ARRAY),
            .dtor = meta->dtor,
            .meta = { arr_meta, *metasize },
        };
    } else {
        void *user_meta = tr24sp__get_smart_ptr_meta(ptr);
        args = (tr24sp__s_smalloc_args){
            .size = size,
            .kind = TR24SP__SHARED,
            .dtor = meta->dtor,
            .meta = { user_meta, *metasize },
        };
    }

    void *newptr = tr24sp__smalloc(&args);
    TR24_MEMCPY(newptr, ptr, size);
    return newptr;
}

TR24_MALLOC_API
TR24_INLINE static void *alloc_entry(size_t head, size_t size, size_t metasize)
{
    const size_t totalsize = head + size + metasize + sizeof(size_t);
#ifdef SMALLOC_FIXED_ALLOCATOR
    return TR24_MALLOC(totalsize);
#else /* !SMALLOC_FIXED_ALLOCATOR */
    return tr24sp__smalloc_allocator.alloc(totalsize);
#endif /* !SMALLOC_FIXED_ALLOCATOR */
}

TR24_INLINE static void tr24sp__dealloc_entry(tr24sp__s_meta *meta, void *ptr)
{
    if(meta->dtor) {
        void *user_meta = tr24sp__get_smart_ptr_meta(ptr);
        if(meta->kind & TR24SP__ARRAY) {
            tr24sp__s_meta_array *arr_meta = (void *)(meta + 1);
            for(size_t i = 0; i < arr_meta->nmemb; ++i)
                meta->dtor((char *)ptr + arr_meta->size * i, user_meta);
        } else
            meta->dtor(ptr, user_meta);
    }

#ifdef SMALLOC_FIXED_ALLOCATOR
    TR24_FREE(meta);
#else /* !SMALLOC_FIXED_ALLOCATOR */
    tr24sp__smalloc_allocator.dealloc(meta);
#endif /* !SMALLOC_FIXED_ALLOCATOR */
}

TR24_MALLOC_API
static void *tr24sp__smalloc_impl(tr24sp__s_smalloc_args *args)
{
    if(!args->size)
        return NULL;

    size_t aligned_metasize = tr24sp__align(args->meta.size);
    size_t size = tr24sp__align(args->size);

    size_t head_size = args->kind & TR24SP__SHARED ?
                           sizeof(tr24sp__s_meta_shared) :
                           sizeof(tr24sp__s_meta);
    tr24sp__s_meta_shared *ptr = alloc_entry(head_size, size, aligned_metasize);
    if(ptr == NULL)
        return NULL;

    char *shifted = (char *)ptr + head_size;
    if(args->meta.size && args->meta.data)
        TR24_MEMCPY(shifted, args->meta.data, args->meta.size);

    size_t *sz = (size_t *)(shifted + aligned_metasize);
    *sz = head_size + aligned_metasize;

    *(tr24sp__s_meta *)ptr = (tr24sp__s_meta){ .kind = args->kind,
                                               .dtor = args->dtor,
#ifndef NDEBUG
                                               .ptr = sz + 1
#endif
    };

    if(args->kind & TR24SP__SHARED)
        ptr->ref_count = 1;

    return sz + 1;
}

TR24_MALLOC_API
TR24_INLINE static void *tr24sp__smalloc_array(tr24sp__s_smalloc_args *args)
{
    const size_t size =
        tr24sp__align(args->meta.size + sizeof(tr24sp__s_meta_array));
    char new_meta[size];
    tr24sp__s_meta_array *arr_meta = (void *)new_meta;
    *arr_meta = (tr24sp__s_meta_array){
        .size = args->size,
        .nmemb = args->nmemb,
    };
    TR24_MEMCPY(arr_meta + 1, args->meta.data, args->meta.size);
    return tr24sp__smalloc_impl(&(tr24sp__s_smalloc_args){
        .size = args->nmemb * args->size,
        .kind = (enum tr24sp__pointer)(args->kind | TR24SP__ARRAY),
        .dtor = args->dtor,
        .meta = { &new_meta, size },
    });
}

TR24_MALLOC_API
void *tr24sp__smalloc(tr24sp__s_smalloc_args *args)
{
    return (args->nmemb == 0 ? tr24sp__smalloc_impl :
                               tr24sp__smalloc_array)(args);
}

void tr24sp__sfree(void *ptr)
{
    if(!ptr)
        return;

    TR24_ASSERT((size_t)ptr == tr24sp__align((size_t)ptr));
    tr24sp__s_meta *meta = tr24sp__get_meta(ptr);
    TR24_ASSERT(meta->ptr == ptr);

    if(meta->kind & TR24SP__SHARED &&
       atomic_decrement(&((tr24sp__s_meta_shared *)meta)->ref_count))
        return;

    tr24sp__dealloc_entry(meta, ptr);
}

void *tr24sp__srealloc(size_t type, void *ptr, size_t size)
{
    if(!ptr)
        return NULL;
    TR24_ASSERT((size_t)ptr == tr24sp__align((size_t)ptr));
    tr24sp__s_meta *meta = tr24sp__get_meta(ptr);
    TR24_ASSERT(ptr == ptr);
    void *newptr = tr24sp__smalloc_m(type, size / type, meta->kind, meta->dtor);
    TR24_MEMCPY(newptr, ptr, size);
    tr24sp__sfree(ptr);
    return newptr;
}

#endif /* TR24_SMARTPTR_H_ */

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
