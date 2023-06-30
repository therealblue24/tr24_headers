#define TR24_IMPL
#include "../tr24_valid_ptr.h"
#include <stdlib.h>
#include <stdio.h>

int main()
{
    int *ptr1 = (int *)0xfeedface;
    void *ptr2 = malloc(20);
    if(tr24_is_valid(ptr1, 50)) {
        printf("ptr1 is valid\n");
    } else {
        printf("ptr1 is invalid\n");
    }
    if(tr24_is_valid(ptr2, 20)) {
        printf("ptr2 is valid\n");
    } else {
        printf("ptr2 is invalid\n");
    }
    free(ptr2);
}
