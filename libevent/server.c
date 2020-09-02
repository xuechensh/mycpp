#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <arpa/inet.h>

void read_cb(struct bufferevent* bev, void* arg)
{
    char msg[1024] = {0};
    char* ip = (char*) arg;

    bufferevent_read( bev, msg, sizeof(msg)-1);
    printf("Receive from %s msg: %s\n", ip, msg);

    bufferevent_write( bev, msg, strlen( msg));

    return;
}

void write_cb(struct bufferevent* bev, void* arg)
{
    printf("I am server, success write to client\n");
    
    return;
}

void event_cb(struct bufferevent* bev, short events, void* arg)
{
    char* ip = (char*) arg;

    if( events & BEV_EVENT_EOF)
    {
        printf("Client %s close client\n", ip);
    }
    else if( events & BEV_EVENT_ERROR)
    {
        printf("Some othre error\n");
    }
    else
    {
        printf("unknow exit type\n");
    }

    bufferevent_free( bev);
    return;
}

void listen_cb( struct evconnlistener* listener,
                evutil_socket_t fd,
                struct sockaddr* addr, 
                int len,
                void* ptr)
{
    struct sockaddr_in* client = (struct sockaddr_in*) addr;

    printf("New client :%s connect\n", inet_ntoa(client->sin_addr));

    struct event_base* base= (struct event_base*) ptr;

    struct bufferevent* bev = NULL;
    bev = bufferevent_socket_new( base, fd, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb( bev, read_cb, write_cb, event_cb, inet_ntoa(client->sin_addr));

    bufferevent_enable( bev, EV_READ);

    return;
}

int main(int argc, char* argv[])
{
    struct sockaddr_in serveraddr;
    memset( &serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(9999);

    struct event_base* base = NULL;
    base = event_base_new();

    struct evconnlistener* listener = NULL;
    listener = evconnlistener_new_bind( base, listen_cb, base,  LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 
                                        36, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    event_base_dispatch( base);
    evconnlistener_free( listener);
    event_base_free( base);

    return 0;
}

