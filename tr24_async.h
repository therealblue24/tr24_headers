/* tr24_async.h - v0.01 - public domain therealblue24 2023
 * Simple async operations for C
 * 
 * This file provides both the interface and the implementation.
 * To init the implementation,
 *      #define TR24_ASYNC_IMPL or
 *      #define TR24_IMPL
 * in *one* source file, before #including to generate the implementation.
 *
 * Examples are in examples folder.
 *
 * History:
 *      0.02 wrapped tr24_async_t that you can await with no worries
 *      0.01 first public release
 */
#ifndef TR24_ASYNC_H_
#define TR24_ASYNC_H_

#ifdef async
#undef async
#endif /* async */
#define async

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <pthread.h>

#ifndef TR24_MEMCPY
#define TR24_MEMCPY memcpy
#endif /* TR24_MEMCPY */

#ifndef TR24_ASSERT
#define TR24_ASSERT assert
#endif /* TR24_ASSERT */

#ifndef TR24_MALLOC
#define TR24_MALLOC malloc
#endif /* TR24_MALLOC */

#ifndef TR24_FREE
#define TR24_FREE free
#endif /* TR24_FREE */

typedef struct tr24_future {
    int __start_canary;
    pthread_t thread;
    pthread_attr_t attr;
    void *(*func)(void *arg);
    int id;
    void *internal_arg;
    void (*await)(struct tr24_future *future, void *val);
    int __end_canary;
} tr24_future_t;

typedef struct tr24_future_arg {
    int __start_canary;
    void *(*func)(void *arg);
    void *arg;
    int __end_canary;
} tr24_future_arg_t;

typedef struct tr24_promise {
    int __start_canary;
    void *result;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool done;
    int id;
    int __end_canary;
} tr24_promise_t;

typedef struct tr24_async {
    int __start_canary;
    tr24_promise_t *promise;
    tr24_future_t *future;
    void (*await)(struct tr24_async *env);
    int __end_canary;
} tr24_async_t;

tr24_future_t *tr24_future_create(void *(*start_routine)(void *arg));
void tr24_future_start(tr24_future_t *future, void *arg);
void tr24_future_stop(tr24_future_t *future);
void tr24_future_destroy(tr24_future_t *future);
void tr24_future_set_arg(tr24_future_t *future, void *arg);

tr24_promise_t *tr24_promise_create();
void *tr24_promise_get(tr24_promise_t *p);
void tr24_promise_set(tr24_promise_t *p, void *res);
bool tr24_promise_done(tr24_promise_t *p);
void tr24_promise_destroy(tr24_promise_t *p);

tr24_async_t *tr24_async_env(tr24_future_t *future, tr24_promise_t *promise);
void tr24_async_destroy(tr24_async_t *async);

void tr24_await(void *f, void *v);

#ifdef __cplusplus
}
#endif

#endif /* TR24_ASYNC_H_ */

#ifdef TR24_IMPL
#define TR24_ASYNC_IMPL
#endif /* TR24_IMPL */

#ifdef TR24_ASYNC_IMPL
#undef TR24_ASYNC_IMPL

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>

static int __global_id_thingy = 0;

static void _tr24_await_impl_future(tr24_future_t *future, void *val)
{
    tr24_future_set_arg(future, val);
    tr24_future_start(future, future->internal_arg);
}

static void _tr24_await_impl_async(tr24_async_t *async_env)
{
    _tr24_await_impl_future(async_env->future, async_env->promise);
}

void tr24_await(void *f, void *v)
{
    if(((tr24_future_t *)f)->__start_canary == 1) {
        _tr24_await_impl_future(f, v);
    }
    if(((tr24_async_t *)f)->__start_canary == 5) {
        _tr24_await_impl_async(f);
    }
}

tr24_async_t *tr24_async_env(tr24_future_t *future, tr24_promise_t *promise)
{
    tr24_async_t *ret = (tr24_async_t *)TR24_MALLOC(sizeof(tr24_async_t));
    ret->__start_canary = 5;
    ret->__end_canary = 5;
    ret->await = _tr24_await_impl_async;
    ret->future = future;
    ret->promise = promise;
    return ret;
}

void tr24_async_destroy(tr24_async_t *_async)
{
    tr24_promise_destroy(_async->promise);
    tr24_future_destroy(_async->future);
    TR24_FREE(_async);
}

tr24_future_t *tr24_future_create(void *(*start_routine)(void *arg))
{
    tr24_future_t *future = (tr24_future_t *)TR24_MALLOC(sizeof(tr24_future_t));
    pthread_attr_init(&future->attr);
    pthread_attr_setdetachstate(&future->attr, PTHREAD_CREATE_JOINABLE);
    future->func = start_routine;
    future->id = __global_id_thingy;
    future->await = _tr24_await_impl_future;
    srand(time(NULL));
    __global_id_thingy += rand();
    future->__start_canary = 1;
    future->__end_canary = 1;
    return future;
}

void tr24_future_set_arg(tr24_future_t *future, void *arg)
{
    if(arg) {
        future->internal_arg = arg;
    }
}

static void *tr24_future_func_wrapper(void *arg)
{
    tr24_future_arg_t *f = (tr24_future_arg_t *)arg;
    void *res = f->func(f->arg);
    f->__end_canary = 2;
    f->__start_canary = 2;
    TR24_FREE(f);
    pthread_exit(res);
    return res;
}

void tr24_future_start(tr24_future_t *future, void *arg)
{
    tr24_future_arg_t *future_arg =
        (tr24_future_arg_t *)TR24_MALLOC(sizeof(tr24_future_arg_t));
    future_arg->func = future->func;
    future_arg->arg = arg;
    pthread_create(&future->thread, &future->attr, tr24_future_func_wrapper,
                   future_arg);
}

void tr24_future_stop(tr24_future_t *future)
{
    pthread_cancel(future->thread);
}

void tr24_future_destroy(tr24_future_t *future)
{
    void *status;
    int rc = pthread_join(future->thread, &status);
    pthread_attr_destroy(&future->attr);
    __global_id_thingy = future->id;
    TR24_FREE(future);
}

tr24_promise_t *tr24_promise_create()
{
    tr24_promise_t *promise = (tr24_promise_t *)malloc(sizeof(tr24_promise_t));
    pthread_mutex_init(&promise->mutex, NULL);
    pthread_cond_init(&promise->cond, NULL);
    promise->id = __global_id_thingy;
    srand(time(NULL));
    __global_id_thingy += rand();
    promise->__start_canary = 3;
    promise->__end_canary = 3;
    return promise;
}

void tr24_promise_set(tr24_promise_t *p, void *res)
{
    pthread_mutex_lock(&p->mutex);
    p->result = res;
    p->done = true;
    pthread_cond_signal(&p->cond);
    pthread_mutex_unlock(&p->mutex);
}

void *tr24_promise_get(tr24_promise_t *p)
{
    pthread_mutex_lock(&p->mutex);
    while(!p->done) {
        pthread_cond_wait(&p->cond, &p->mutex);
    }
    pthread_mutex_unlock(&p->mutex);
    return p->result;
}

bool tr24_promise_done(tr24_promise_t *p)
{
    pthread_mutex_lock(&p->mutex);
    bool done = p->done;
    pthread_mutex_unlock(&p->mutex);
    return done;
}

void tr24_promise_destroy(tr24_promise_t *p)
{
    pthread_mutex_destroy(&p->mutex);
    pthread_cond_destroy(&p->cond);
    TR24_FREE(p);
}

#ifdef __cplusplus
}
#endif

#endif /* TR24_ASYNC_IMPL */

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
