#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int g_iNumber = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;

void* thread_read(void* arg)
{
    srand(time(NULL));
    int num = *((int*)arg);
    int i;

    for( i = 0; i < 5; i++){
        sleep(rand() % 5);
        printf("thread %d ready to delete\n", num);
        pthread_mutex_lock( &mutex);
        while( g_iNumber == 0)
        {
            pthread_cond_wait( &cond, &mutex);
        }
    
        g_iNumber -= 1;

        printf("thread %d use one condition result = %d\n", num, g_iNumber);
        pthread_mutex_unlock( &mutex);
    }

    int *result = malloc(sizeof(int));
    *result = num;
    return (void*)result;
}

void* thread_write(void* arg)
{
    srand(time(NULL));
    int num = *((int*)arg);
    int i;

    for( i = 0; i < 30; ++i)
    {
        pthread_mutex_lock( &mutex);
        g_iNumber += 1;
        printf("pthread No.%d id=%lu add one cond result = %d\n", num, pthread_self(), g_iNumber);
        pthread_cond_signal( &cond);
        pthread_mutex_unlock( &mutex);
        sleep( random()%5);
    }

    int *result = malloc(sizeof(int));
    *result = num;
    return (void*)result;
}


int main( int argc, char* argv[])
{
    pthread_mutex_init( &mutex, NULL);
    pthread_cond_init( &cond, NULL);
    
    pthread_t pids[10];
    int pno[10];
    int i = 0;
    for( i = 0; i < 2; ++i)
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
        printf("thread %d exit return %d\n", pno[i], *result);
        free(result);
    }
    
    pthread_mutex_destroy( &mutex);
    pthread_cond_destroy( &cond);
    return 0;
}

