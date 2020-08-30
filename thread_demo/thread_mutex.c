#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int g_iNum = 0;
pthread_mutex_t mutex;

void* change(void* arg)
{
    int pthid = *((int*)arg);
    pthread_mutex_lock( &mutex);
    printf("thread %d visit num %d\n", pthid, g_iNum);
    g_iNum = pthid;
    printf("thread %d change num to %d\n", pthid, g_iNum);
    pthread_mutex_unlock( &mutex);

    int* result = malloc(sizeof(int));
    *result = pthid;

    return (void*) result;
}

int main( int argc, char* argv[])
{
    pthread_mutex_init( &mutex, NULL);

    pthread_t pthid[10] = {0};
    int pno[10] = {0};
    int i;
    for( i = 0; i < 10; ++i)
    {
        pno[i] = i + 1;
        pthread_create( &pthid[i], NULL, change, &pno[i]);
    }

    int *result;
    for( i = 0; i < 10; ++i)
    {
        pthread_join( pthid[i], (void**)&result);
        printf("ptdid = %d return %d\n", pno[i], *result);
        free( result);
    }

    return 0;
}

