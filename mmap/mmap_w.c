#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(){
    int fd;
    fd = open("./message.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
    if( fd < 0 ){
        perror("open file error");
        exit(-1);
    }

    int ret = ftruncate( fd, 100);
    if( ret < 0 ){
        perror("trunc error");
        exit(-1);
    }

    void *p = mmap( NULL, 100, PROT_READ | PROT_WRITE, MAP_SHARED , fd, 0);
    if( p == MAP_FAILED ){
        perror("mmap error");
        exit(-1);
    }
    close(fd);
    memset( p, 0, 100);

    char *msg = (char*)p;

    int i;
    for( i = 0; i < 30; i ++){
        sprintf(msg, "Write %d\n\0", i);
        sleep(2);
    }

    munmap( p, 100);
    unlink("./message.txt");

    return 0;
}

