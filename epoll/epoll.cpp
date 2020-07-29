#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define EPOLL_MAX_LISTEN_SIZE (1000)

#define SERVER_PORT (9999)
#define SERVER_MAX_LISTEN_LEN (30)

#define MSG_SIZE (5)

using namespace std;

int main(int argc, char* argv[]){
    //create listen fd
    int listen_fd = socket(PF_INET, SOCK_STREAM, 0);
    if ( -1 == listen_fd ){
        cout << "create listen fd error" << endl;
        return -1;
    }
    
    //create addr
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    //bind server addr to listen fd
    if ( -1 == bind( listen_fd, (sockaddr*)&server_addr, sizeof(server_addr)) ){
        cout << "bind server addr to listen fd error" << endl;
        close(listen_fd);
        return -1;
    }
    
    //listen server fd
    if ( -1 == listen( listen_fd, SERVER_MAX_LISTEN_LEN) ){
        cout << "listen server fd error" << endl;
        close(listen_fd);
        return -1;
    }

    //create epoll fd
    int epoll_fd = epoll_create(EPOLL_MAX_LISTEN_SIZE);
    if ( -1 == epoll_fd ){
        cout << "create epoll fd error" << endl;
        close(listen_fd);
        return -1;
    }

    //add dest fd to epoll fd
    struct epoll_event ev;
    ev.data.fd = listen_fd;
    ev.events = EPOLLIN;
    int add_result = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);
    if ( -1 == add_result ){
        cout << "add listenfd to epollfd error" << endl;
        close(listen_fd);
        close(epoll_fd);
        return -1;
    }
    
    //create epoll event list
    struct epoll_event evs[EPOLL_MAX_LISTEN_SIZE];

    //start
    while( 1 ){
        //epoll work
        int active_cnt = epoll_wait(epoll_fd, evs, EPOLL_MAX_LISTEN_SIZE, -1);
        if ( active_cnt < 0 ){
            cout << "epoll error" <<endl;
            break;
        }

        for ( int i = 0; i < active_cnt;++i ){
            //listen fd
            if ( evs[i].data.fd == listen_fd ){
                struct sockaddr_in client_addr;
                socklen_t sockaddr_len;
                int client_fd = accept(listen_fd, (sockaddr*)&client_addr, &sockaddr_len);
                cout << "client fd:" << client_fd << " port:" << ntohs(client_addr.sin_addr.s_addr) << endl;

                //add client client fd to epoll fd
                struct epoll_event client_ev;
                client_ev.data.fd = client_fd;
                client_ev.events = EPOLLIN;
                int client_add_result = epoll_ctl( epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev);
                if ( -1 == client_add_result ){
                    cout << "add " << client_fd << " to epoll fd error" << endl;
                    close(client_fd);
                    continue;
                }
                
                char message[MSG_SIZE];
                bzero(message, MSG_SIZE);
                sprintf(message, "Hi\n");
                write(client_fd, message, strlen(message));
            }
            // client fd
            else{
                int client_fd = evs[i].data.fd;
                char message[MSG_SIZE];
                bzero(message, MSG_SIZE);
                int read_len = read( client_fd, message, MSG_SIZE-1);
                if ( 0 == read_len ){
                    //client fd close
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
                    close(client_fd);
                    continue;
                }
                else{
                    message[MSG_SIZE - 1] = '\0';
                    cout << "Client:" << client_fd << " send:" << message << endl;

                    bzero(message, MSG_SIZE);
                    sprintf(message, "Next");
                    write(client_fd, message, strlen(message));
                }
            }
        }
    }
    close(listen_fd);
    close(epoll_fd);
    return 0;
}

