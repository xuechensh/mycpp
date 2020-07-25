#include <stdio.h>
#include <string.h>

#define sn_memzero(buf ,n) (void)memset(buf, 0, n)
#define sn_free free

void* sn_alloc(size_t size);
void* sn_calloc(size_t size);

