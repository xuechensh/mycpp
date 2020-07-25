#include <stdlib.h>
#include "sn_alloc.h"

void* sn_alloc(size_t size)
{
    void* p = NULL;
    p = malloc(size);
    return p;
}

void* sn_calloc(size_t size)
{
    void* p = NULL;

    p = sn_alloc(size);

    if( p )
    {
        sn_memzero(p, size);
    }

    return p;
}


