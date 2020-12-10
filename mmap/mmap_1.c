#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(){
    void *p = mmap( NULL, 100, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    if( p == MAP_FAILED ){
        perror("mmap error");
        exit(-1);
    }

    char *msg = (char*)p;
    pid_t pid = fork();
    if( pid < 0 ){
        perror("fork error");
        exit(-1);
    } else if ( pid == 0 ){
        sleep(5);
        msg[99] = 0;
        printf("Children read %s\n", msg);
        strcpy( msg, "Children Receive");
    } else {
        strcpy( msg, "Hello");
        printf("Father write\n");
        wait(NULL);
        printf("Father read %s\n", msg);
    }

    munmap( p, 100);

    return 0;
}

