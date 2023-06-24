#define TR24_IMPL
#include "../tr24_async.h"

int *v = NULL;

// the async function
// (the async macro does nothing, it is only for decoration)
async void *func(void *arg)
{
    tr24_promise_t *p = (tr24_promise_t *)arg;
    printf("started thread\n");
    sleep(3);
    printf("thread sets promise\n");
    tr24_promise_set(p, v);
    printf("stopping thread\n");
    return NULL;
}

int main()
{
    int tmp = 42;
    v = &tmp;
    tr24_future_t *future = tr24_future_create(func);
    tr24_promise_t *promise = tr24_promise_create();
    tr24_async_t *env = tr24_async_env(future, promise);
    env->await(env);
    tr24_await(env, NULL);
    printf("ba ba black sheep i am waiting\n");
    printf("I got: %d", *(int *)tr24_promise_get(env->promise));
    tr24_async_destroy(env);
}
