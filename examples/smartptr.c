#include <stdio.h>
#define TR24_SMARTPTR_IMPL
#include "../tr24_smartptr.h"

void cleanup(void *ptr, void *meta)
{
    (void)meta;
    printf("%d\n", *(int *)ptr);
}

int main()
{
    tr24_smart int *some_int = tr24_unique_ptr(int[3], { 42, 32, 12 }, cleanup);
    /* No more memory leaks. */
    return 0;
}
