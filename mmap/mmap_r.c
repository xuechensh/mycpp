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
    fd = open("./message.txt", O_RDONLY);
    if( fd < 0 ){
        perror("open error");
        exit(-1);
    }

    void *p = mmap( NULL, 100, PROT_READ, MAP_SHARED, fd, 0);
    if( p == MAP_FAILED ){
        perror("mmap error");
        exit(-1);
    }
    close(fd);

    char *msg = (char*)p;
    
    int i;
    for( i = 0; i < 5; i++){
        printf("Read %s\n", msg);
        sleep(random() % 3);
    }

    munmap( p, 100);

    return 0;
}

