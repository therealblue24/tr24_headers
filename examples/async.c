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
    printf("main thread\n");
    // create future and promise
    tr24_future_t *f = tr24_future_create(func);
    tr24_promise_t *p = tr24_promise_create();
    // using the sugar syntax
    f->await(f, p);
    // or the function
    tr24_await(f, p);
    printf("ba ba black sheep I am waiting\n");
    printf("got result from future: %d\n", *(int *)tr24_promise_get(p));
    tr24_future_close(f);
    tr24_promise_close(p);
}
