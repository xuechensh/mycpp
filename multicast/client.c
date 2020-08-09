#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>

int main(int argc, char* argv[])
{
    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    if ( fd == -1 )
    {
        perror("create fd error");
        return -1;
    }

    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &client_addr.sin_addr.s_addr);
    client_addr.sin_port = htons(8989);
    if ( -1 == bind( fd, (struct sockaddr*)&client_addr, sizeof(client_addr)) )
    {
        perror("bind address to client fd error");
        return -1;
    }

    struct ip_mreqn flag;
    memset(&flag, 0, sizeof(flag));
    inet_pton(AF_INET, "239.0.0.10", &flag.imr_multiaddr.s_addr);
    inet_pton(AF_INET, "0.0.0.0", &flag.imr_address.s_addr);
    flag.imr_ifindex = if_nametoindex("enp0s3");
    setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &flag, sizeof(flag));

    while(1)
    {
        char msg[1024] = {0};
        int len = recvfrom(fd, msg, sizeof(msg), 0, NULL, NULL);
        if ( len == -1)
        {
            perror("recvfrom error");
            break;
        }
        msg[1023] = '\0';
        printf("client recv: %s\n", msg);
    }
    
    close(fd);
}
   
