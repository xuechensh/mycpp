#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_THREADS (5)

int main(int argc, char* argv[]){
    if( argc != 3 ){
        perror("argc error");
        exit(-1);
    }

    int ret;
    
    /* create sorcre file fd*/
    const char *fileSource = argv[1];
    int fs = open( fileSource, O_RDONLY);
    if( fs == -1 ){
        perror("open source file error");
        exit(-1);
    }

    /* create dest file fd */
    const char *fileDest = argv[2];
    int fd = open( fileDest, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if( fd == -1 ){
        perror("open dest file error");
        exit(-1);
    }

    /* get source file size  */
    struct stat s_buf;
    ret = fstat( fs, &s_buf);
    if( ret < 0 ){
        perror("file source stat get error");
        exit(-1);
    }

    int size = s_buf.st_size;

    /* set dest file size */
    ret = ftruncate( fd, size);
    if( ret < 0 ){
        perror("ftruncate fd error");
        exit(-1);
    }

    /*creat source file mmap and close fd */
    char * const ps = mmap( NULL, size, PROT_READ, MAP_SHARED, fs, 0);
    if( ps == MAP_FAILED ){
        perror("mmap fs error");
        exit(-1);
    }
    close(fs);

    /* create dest file mmap and close fd */
    char * const pd = mmap( NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if( pd == MAP_FAILED ){
        perror("mmap fd error");
        exit(-1);
    }
    close(fd);
    
    /*process copy*/
    if( size >= MAX_THREADS ){
        int bs = size / MAX_THREADS;
        int mod = size % MAX_THREADS;

        int i;
        pid_t pid;
        
        for( i = 0; i < MAX_THREADS; i++){
            pid = fork();
            if( pid < 0 ){
                perror("fork error");
                int ii;
                for( ii = 0; ii < i; ii++){
                    wait(NULL);
                }
                exit(-1);
            } else if ( pid == 0 ){
                break;
            }
        }

        if( i == MAX_THREADS - 1 ){
            memcpy( pd+i*bs, ps+i*bs, bs+mod);
        } else if( i == MAX_THREADS ){
            for(i = 0; i < MAX_THREADS; i++){
                wait(NULL);
            }
        } else {
            memcpy( pd+i*bs, ps+i*bs, bs);
        }
    } else {
        memcpy( pd, ps, size);
    }

    munmap( ps, size);
    munmap( pd, size);

    return 0;
}

