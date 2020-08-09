//include head file
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char* argv[])
{
    //create lfd
    int lfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if ( lfd == -1 )
    {
        perror("creat lfd error");
        exit(-1);
    }

    //if server.socket exit delete
    unlink("server.socket");

    //create server file and bind
    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "server.socket");
    if ( -1 == bind( lfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) )
    {
        perror("bind server file to lfd error");
        exit(-1);
    }

    //listen lfd
    if( -1 == listen( lfd, 30) )
    {
        perror("listen lfd error");
        exit(-1);
    }

    //create client addr
    struct sockaddr_un client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t len = sizeof(client_addr);
    
    //accept connect
    int cfd = accept( lfd, (struct sockaddr*)&client_addr, &len);
    if ( cfd == -1 )
    {
        perror("accept cfd error");
        exit(-1);
    }
    printf("======client bind file: %s\n", client_addr.sun_path);

    //communication
    char msg[1024];
    while(1)
    {
        memset( msg, 0, sizeof(msg));
        int recvlen = recv( cfd, msg, sizeof(msg), 0);
        if ( recvlen == -1 )
        {
            perror("recv msg error");
            break;
        }
        else if ( recvlen == 0 )
        {
            printf("client close connection\n");
            break;
        }
        else
        {
            msg[1023] = '\0';
            printf("recv msg from client len: %d:%s", recvlen, msg);
            send( cfd, msg, strlen(msg)+1, 0);
        }
    }

    close(lfd);
    close(cfd);

    return 0;
}

