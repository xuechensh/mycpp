#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>

int main(int grac, char* argv[])
{
    int fd = socket( PF_INET, SOCK_DGRAM, 0);
    if ( fd == -1 )
    {
        perror("socket creat error");
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0 ,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(8787);
    if ( -1 == bind( fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) )
    {
        perror("bind server addr to socket error");
        return -1;
    }

    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = 8989;
    inet_pton(AF_INET, "239.0.0.10", &client_addr.sin_addr.s_addr);

    struct ip_mreqn flag;
    inet_pton(AF_INET, "239.0.0.10", &flag.imr_multiaddr.s_addr);
    inet_pton(AF_INET, "0.0.0.0", &flag.imr_address.s_addr);
    flag.imr_ifindex = if_nametoindex("eth0");
    setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &flag, sizeof(flag));

    while(0)
    {
        static int num = 0;
        char msg[1024] = {0};
        sprintf( msg, "hello to multicast network, hello time = %d", num);
        int ret = sendto(fd, msg, sizeof(msg)+1, 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
        if( ret == -1 )
        {
            perror("sendto error");
            break;
        }

        printf("send msg is: %s\n", msg);
        sleep(1);
    }
    
    close(fd);

    return 0;
}

