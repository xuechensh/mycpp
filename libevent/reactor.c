#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_EVENTS  (1024)
#define SERVER_PORT (8888)
#define BUFSIZE (1024)
#define MSGLEN (BUFSIZE-1)

//define my events struct
struct MyEvent{
    int m_fd;   //listen sock fd
    int m_event;  //epoll event
    void* m_arg;    //point to self
    void (*call_back)(int fd, int event, void* arg);    //call back func
    char m_buf[BUFSIZE];
    int m_buf_len;
    int m_status;
    time_t m_lasttime;  //last caozuo time
};

int ep_fd;
struct MyEvent ep_events[MAX_EVENTS];

//shengming init listen socket
void initlistensocket(int ep_fd, unsigned short port);

//init my event
void eventset( struct MyEvent *my_ev, int fd, void (*call_back)(int, int, void*), void* event_arg);

//add my events to epfd
void eventadd( int epfd, int event, struct MyEvent *ev);

//delete my event from epfd
void eventdel( int epfd, struct MyEvent *ev);

//send data
void senddata( int client_fd, int event, void* arg);

//recv data
void recvdata( int client_fd, int event, void* arg);

//accept connection
void acceptconnection( int lfd, int event, void*arg);

void loop()
{
    unsigned short port = SERVER_PORT;

    ep_fd = epoll_create(MAX_EVENTS);
    if ( ep_fd == -1 )
    {
        printf("create epfd in function %s error info = %s\n", __func__, strerror(errno));
        return;
    }

    initlistensocket( ep_fd, port);
    
    int checkpos = 0;
    int i;
    struct epoll_event events[MAX_EVENTS];

    while(1)
    {
        long now = time(NULL);

        for ( i = 0; i < 100; i++, checkpos++)
        {
            if( checkpos == MAX_EVENTS - 1)
            {
                checkpos = 0;
            }

            if(ep_events[checkpos].m_status == 0 )
            {
                continue;
            }

            long spell_time = now - ep_events[i].m_lasttime;
            if( spell_time >= 60 )
            {
                printf("fd:%d timeout,close\n", ep_events[i].m_fd);
                eventdel( ep_fd, &ep_events[i]);
                close( ep_events[i].m_fd);

            }
        }

        int n_ready = epoll_wait( ep_fd, events, MAX_EVENTS, 1000);
        if( n_ready < 0 )
        {
            printf("epoll wait error, exit\n");
            break;
        }

        for( i = 0; i < n_ready; ++i )
        {
            struct MyEvent *ev = (struct MyEvent*)(events[i].data.ptr);
            if( (ev->m_event & EPOLLIN) && (events[i].events & EPOLLIN) )
            {
                ev->call_back( ev->m_fd, events[i].events, ev->m_arg);
            }
            if( (ev->m_event & EPOLLOUT) && (events[i].events & EPOLLOUT) )
            {
                ev->call_back( ev->m_fd, events[i].events, ev->m_arg);
            }
        }

    }
    return;
}


void initlistensocket( int ep_fd, unsigned short port)
{
    int lfd = socket( PF_INET, SOCK_STREAM, 0);
    if ( lfd == -1 )
    {
        printf("create lfd error in function %s \n", __func__);
        return;
    }

    //set reuse addr
    int opt = 1;
    setsockopt( lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //set non block
    fcntl( lfd, F_SETFL, O_NONBLOCK);

    struct sockaddr_in server_addr;
    memset( &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if( -1 == bind( lfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) )
    {
        printf("bind server to lfd error in function %s \n", __func__);
        return;
    }

    if( -1 == listen( lfd, 128) )
    {
        printf("listen lfd error in function %s \n", __func__);
        return;
    }

    //init lfd my events
    eventset( &ep_events[MAX_EVENTS-1], lfd, acceptconnection, &ep_events[MAX_EVENTS-1]);

    //add to ep fd
    eventadd( ep_fd, EPOLLIN|EPOLLET, &ep_events[MAX_EVENTS-1]);

    return;
}

void eventset( struct MyEvent *my_ev, int fd, void (*call_back)(int, int, void*), void* event_arg)
{
    my_ev->m_fd = fd;
    my_ev->m_event = 0; //when init do not know event
    my_ev->m_arg = event_arg;
    my_ev->call_back = call_back;
    my_ev->m_status = 0;
    my_ev->m_lasttime = time(NULL);
    
    return;
}


void eventadd( int ep_fd, int event, struct MyEvent *my_ev)
{
    struct epoll_event epv;
    epv.data.ptr = my_ev;
    epv.events = my_ev->m_event = event;

    int op;
    if( my_ev->m_status == 0 )
    {
        op = EPOLL_CTL_ADD;
    }
    else
    {
        printf("my event in already on the tree\n");
        return;
    }

    if( -1 == epoll_ctl( ep_fd, op, my_ev->m_fd, &epv) )
    {
        printf("in func %s add node to ep fd error\n", __func__);
        return;
    }
    
    my_ev->m_status = 1;

    return;
}

//del node from ep_fd
void eventdel( int ep_fd, struct MyEvent* ev)
{
    if ( ev->m_status != 1 )
    {
        return;
    }

    epoll_ctl( ep_fd, EPOLL_CTL_DEL, ev->m_fd, NULL);
    ev->m_status = 0;

    return;
}

//acceptconnection
void acceptconnection(int lfd, int event, void *arg)
{
    struct sockaddr_in client_addr;
    socklen_t socklen;

    int cfd = accept( lfd, (struct sockaddr*)&client_addr, &socklen);
    if ( cfd == -1 )
    {
        printf("int function %s accept connect error %s\n", __func__, strerror(errno));
        return;
    }

    int i;
    for( i = 0; i < MAX_EVENTS; ++i)
    {
        if ( ep_events[i].m_status == 0 )
        {
            break;
        }
    }

    if ( i == MAX_EVENTS )
    {
        close( lfd );
        printf("ep_events max\n");
        return;
    }

    int flag = fcntl( cfd, F_SETFL, O_NONBLOCK);
    if ( flag == -1 )
    {
        close( lfd);
        printf("set cfd no block error\n");
        return;
    }

    eventset( &ep_events[i], cfd, recvdata, &ep_events[i]);
    eventadd( ep_fd, EPOLLIN|EPOLLET, &ep_events[i]);

    char str[BUFSIZE];
    printf("new connect [%s:%d] [time:%ld] [pos:%d]\n",
            inet_ntop( AF_INET, &client_addr.sin_addr.s_addr, str, sizeof(str)),
            ntohs( client_addr.sin_port), 
            ep_events[i].m_lasttime, 
            i);

    return;
}

void recvdata( int cfd, int event, void* arg)
{
    int len = 0;
    int sum = 0;
    struct MyEvent *ev = (struct MyEvent*) arg;
    
    memset( ev->m_buf, 0, BUFSIZE);
    while( (len = recv( cfd, ev->m_buf, MSGLEN, 0)) > 0 )
    {
        sum += len;
        printf("client %d send :%s\n", cfd, ev->m_buf);
        memset( ev->m_buf, 0, strlen(ev->m_buf));
    }
    
    eventdel( ep_fd, ev);

    if ( len == -1 && errno == EAGAIN )
    {
        sprintf( ev->m_buf, "recv len =%d", sum);
        ev->m_buf_len = strlen(ev->m_buf);
        eventset( ev, cfd, senddata, ev);
        eventadd( ep_fd, EPOLLOUT|EPOLLET, ev);
    }
    else
    {
        close( ev->m_fd);
        printf("close connect %d\n", ev->m_fd);
    }

    return;
}


void senddata( int cfd, int event, void* arg)
{
    int len = 0;
    struct MyEvent *ev = (struct MyEvent*) arg;

    len = send( cfd, ev->m_buf, ev->m_buf_len, 0);

    eventdel( ep_fd, ev);

    if ( len > 0 )
    {
        printf("send to client:%d len %d\n", cfd, len);

        eventset( ev, cfd, recvdata, ev);
        eventadd( ep_fd, EPOLLIN|EPOLLET, ev);
    }
    else
    {
        close( cfd );
        printf("close client:%d\n", cfd);
    }

    return;
}

int main(int argc, char* argv[])
{
    loop();
    return 0;
}
