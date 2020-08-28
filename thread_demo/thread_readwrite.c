#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <time.h>
#include <time.h>

int g_iNumber = 0;
pthread_rwlock_t rwlock;

void* thread_read(void* arg)
{
    srand(time(NULL));
    int num = *((int*)arg);
    int i = 0;
    for( i = 0; i < 10; ++i) 
    {
        pthread_rwlock_rdlock(&rwlock);
        printf("pthread No.%d id=%lu read num = %d\n", num, pthread_self(), g_iNumber);
        pthread_rwlock_unlock(&rwlock);
        sleep(rand() % 10);
    }
    
    int *result = malloc(sizeof(int));
    *result = num;
    return (void*)result;
}

void* thread_write(void* arg)
{
    srand(time(NULL));
    int num = *((int*)arg);
    int i = 0;
    for( i = 0; i < 10; ++i)
    {
        pthread_rwlock_wrlock(&rwlock);
        g_iNumber = num;
        printf("pthread No.%d id=%lu set num = %d\n", num, pthread_self(), g_iNumber);
        pthread_rwlock_unlock(&rwlock);
        sleep(rand() % 10);
    }

    int *result = malloc(sizeof(int));
    *result = num;
    return (void*)result;
}


int main( int argc, char* argv[])
{
    pthread_rwlock_init( &rwlock, NULL);
    
    pthread_t pids[10];
    int pno[10];
    int i = 0;
    for( i = 0; i < 3; ++i)
    {
        pno[i] = i + 1;
        pthread_create( &pids[i], NULL, thread_write, &pno[i]);
    }

    for(; i < 10; ++i)
    {
        pno[i] = i + 1;
        pthread_create( &pids[i], NULL, thread_read, &pno[i]);
    }

    int* result;
    for( i = 0; i < 10; ++i)
    {
        pthread_join( pids[i], (void**)&result);
        printf("int main %d thread exit return \n", *result);
        free(result);
    }

    return 0;
}

