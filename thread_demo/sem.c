#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define SIZE (5)

struct Pdt{
    sem_t sem_exit, sem_black;
    sem_t sem_p, sem_c;

    int i, j;

    int value[SIZE];
};

struct Pdt p;

void* producter(void* args){
    int i;

    for(i = 0; i < 15; i ++){
        sem_wait( &(p.sem_black));
        sem_wait( &(p.sem_p));

        p.value[p.i] = random() % 10;
        printf("--Product %d at %d\n", p.value[p.i], p.i);
        p.i = ( p.i + 1 ) % SIZE;

        sem_post( &(p.sem_p));
        sem_post( &(p.sem_exit));

        sleep(random() % 5);
    }
    
}

void* customer(void* args){
    int i;

    for(i = 0; i < 10; i ++){
        sem_wait( &(p.sem_exit));
        sem_wait( &(p.sem_c));

        printf("--Customer %d at %d\n", p.value[p.j], p.j);
        p.value[p.j] = 0;
        p.j = ( p.j + 1 ) % SIZE;

        sem_post( &(p.sem_c));
        sem_post( &(p.sem_black));

        sleep(random() % 5);
    }
    
}

int main(){
    sem_init( &(p.sem_exit), 0, 0);
    sem_init( &(p.sem_black), 0, SIZE);
    sem_init( &(p.sem_p), 0, 1);
    sem_init( &(p.sem_c), 0, 1);

    p.i = p.j = 0;
    memset( &(p.value), 0, sizeof(int) * SIZE);

    pthread_t p1, p2, p3, p4, p5;

    pthread_create( &p1, NULL, producter, NULL);
    pthread_create( &p2, NULL, producter, NULL);
    pthread_create( &p3, NULL, customer, NULL);
    pthread_create( &p4, NULL, customer, NULL);
    pthread_create( &p5, NULL, customer, NULL);

    pthread_join( p1, NULL);
    pthread_join( p2, NULL);
    pthread_join( p3, NULL);
    pthread_join( p4, NULL);
    pthread_join( p5, NULL);

    sem_destroy( &(p.sem_exit));
    sem_destroy( &(p.sem_black));
    sem_destroy( &(p.sem_p));
    sem_destroy( &(p.sem_c));
}

