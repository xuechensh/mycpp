#include <iostream>
#include "mempool.h"
#include "sn_alloc.h"

static void* sn_palloc_small(sn_pool_t* pool, size_t size)
{
    unsigned char* m;
    sn_pool_t *p;

    p = pool->curr;

    do
    {
        m = p->data.last;

        if ( (size_t)(p->data.end - m) >= size )
        {
            p->data.last += size;

            return m;
        }

        p = p->data.next;

    }while(p);

}

static void* sn_palloc_large(sn_pool_t pool, size_t size)
{
    void* p;
    sn_large_pool_t* large;

    p = sn_calloc(size);
    if( NULL == p )
    {
        return NULL;
    }




MemPool::MemPool(size_t size)
{
    sn_pool_t* p;

    p = sn_calloc(size);

    if ( NULL == p)
    {
        return NULL;
    }
    
    p->data.start = (unsigned char*)p;
    p->data.last = (unsigned char*)p + sizeof(sn_pool_t);
    p->data.end = (unsigned char*)p + size;
    p->data.next = NULL;

    p->curr = p;
    size = size - sizeof(sn_pool_t);

    p->max = (size < MAX_ALLOC_FROM_POOL)? size : MAX_ALLOC_FROM_POOL;
    p->large = NULL;

    m_MemPool = p;
    return;
}

