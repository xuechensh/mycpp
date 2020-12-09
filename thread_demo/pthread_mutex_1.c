#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>

struct Int{
    int a;

    pthread_mutex_t mut;
    pthread_mutexattr_t attr;
};

int main(){
    int i;

    struct Int * const p = mmap( NULL, sizeof(struct Int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    if( p == NULL ){
        perror("create mmap error");
        exit(-1);
    }
    memset( p, 0, sizeof(struct Int));

    pthread_mutexattr_init( &(p->attr));
    pthread_mutexattr_setpshared( &(p->attr), PTHREAD_PROCESS_SHARED);
    pthread_mutex_init( &(p->mut), &(p->attr));

    pid_t pid;
    pid = fork();
    if( pid < 0 ){
        perror("fork error");
    } else if( pid == 0 ){
        for( i = 0; i < 10; i++ ){
            pthread_mutex_lock( &(p->mut));

            p->a += 2;
            printf("--Children add to %d\n", p->a);

            pthread_mutex_unlock( &(p->mut));
            sleep(random() % 5);
        }
    } else {
        for( i = 0; i < 10; i++ ){
            pthread_mutex_lock( &(p->mut));

            p->a += 1;
            printf("Father add to %d\n", p->a);

            pthread_mutex_unlock( &(p->mut));
            sleep(random() % 5);
        }
        wait(NULL);
    }
    
    pthread_mutexattr_destroy( &(p->attr));
    pthread_mutex_destroy( &(p->mut));
    munmap( p, sizeof(struct Int));
    return 0;
}
