#include <iostream>

using namespace std;

#define MAX_ALLOC_FROM_POOL

typedef struct sn_pool_s sn_pool_t;
typedef struct sn_pool_large_s sn_pool_large_t;

typedef struct
{
    unsigned char* start;
    unsigned char* last;
    unsigned char* end;
    sn_pool_t* next;
} cn_pool_data_t;

struct sn_pool_large_s
{
    sn_pool_large_t* next;
    void* data;
};

struct sn_pool_s
{
    cn_pool_data_t data;
    sn_pool_t* curr;
    size_t max;
    sn_pool_large_t* large;
};

class MemPool{
public:
    sn_pool_t* MemPool(size_t size);
private:
    sn_pool_t m_MemPool;
};
