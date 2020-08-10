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
    //create cfd
    int cfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if ( cfd == -1 )
    {
        perror("create cfd error");
        exit(-1);
    }

    //if client.socket exit delete
    unlink("client.socket");

    //create client_addr and bind
    struct sockaddr_un client_addr;
    memset( &client_addr, 0, sizeof(client_addr));
    client_addr.sun_family = AF_UNIX;
    strcpy( client_addr.sun_path, "client.socket");
    if( -1 == bind( cfd, (struct sockaddr*)&client_addr, sizeof(client_addr)) )
    {
        perror("bind client_addr to cfd error");
        exit(-1);
    }

    //create server_addr
    struct sockaddr_un server_addr;
    memset( &server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy( server_addr.sun_path, "server.socket");

    //connect
    if ( -1 == connect( cfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) )
    {
        perror("connect to server error");
        exit(-1);
    }

    //communication
    char msg[1024];
    while(1)
    {   
        memset( msg, 0, sizeof(msg));
        fgets( msg, sizeof(msg)-1, stdin);
        printf("send len is %d\n", strlen(msg));
        send( cfd, msg, strlen(msg), 0);
        
        memset( msg, 0, sizeof(msg));
        recv( cfd, msg, sizeof(msg)-1, 0);
        printf("recv from server: %s", msg);
    }

    close(cfd);

    return 0;
}


