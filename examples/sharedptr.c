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
    tr24_smart int *some_int = tr24_shared_ptr(int, 42, cleanup);
    tr24_smart int *other_int = tr24_sref(some_int);
    /* Multiple pointers with shared_ptr! */
    tr24_smart int *unique_ptr = tr24_unique_ptr(int, 42, cleanup);
    /* tr24_smart int *other_ptr = unique_ptr; */
    /* // that piece of code crashes, use shared_ptr for multiple refrences */
    return 0;
}
