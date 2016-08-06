/*
  selectServer.c
  
  Server uses select to monitor sockets for reading and sending
  
  Author Shantanu Jha
  Version: 1.2
*/

#include "servient.h"
#define PORT_NO "2345"

void *get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    else
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main() {
    fd_set master, read_fd;
    int status, listener, new_fd;
    struct addrinfo hints, *ai, *p;
    struct sockaddr_storage addr_in;
    socklen_t addrlen;
    int i, j;
    int fd_max;
    int nbytes;
    char buff[256];
    char remoteIP[INET_ADDRLEN6];
    
    int yes = 1;
    
    FD_ZERO(&master);
    FD_ZERO(&read_fd);
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family =  AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    status = getaddrinfo(NULL, PORT_NO, &hints, &ai);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo:  %s", gai_strerror(status));
        exit(1);
    }
    
    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            perror("listener ");
            continue;
        }
        
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        
        if ((bind(listener, p->ai_addr, p->ai_addrlen)) < 0) {
            close(listener);
            continue;
        }
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind");
        exit(2);
    }
    
    freeaddrinfo(ai);
    
    if ((listen(listener, BACKLOG)) == -1) {
        perror("listen");
        exit(3);
    }
    
    FD_SET(listener, &master);
    fd_max = listener;
    
    printf("Select Server ready...\n");
    
    for (; ;) {
        
        read_fd = master;
        if ((select(fd_max+1, &read_fd, NULL, NULL, NULL)) == -1) {
            perror("select");
            exit(1);
        }
        
        for (i = 0; i <= fd_max; i++) {   // fd_max is all the available sockets
            if (FD_ISSET(i, &read_fd) != 0) {
                if (i == listener) {
                    printf("New Connection...\n");
                    addrlen = sizeof(addr_in);
                    new_fd = accept(listener, (struct sockaddr*)&addr_in, &addrlen);
                    if ( new_fd == -1) {
                        perror("accept");
                    }
                    else {
                        FD_SET(new_fd, &master);
                        if (new_fd > fd_max) {
                            fd_max = new_fd;
                        }
                        printf("selectserver: new connection from %s on socket %d\n", inet_ntop(addr_in.ss_family, get_in_addr((struct sockaddr*)&addr_in), remoteIP, INET_ADDRLEN6), new_fd);
                    }
                }
                else {
                    if ((nbytes = recv(i, buff, sizeof(buff), 0)) <= 0) {
                        if (nbytes == 0) {
                            printf("socket %d hung up\n", i);
                        }
                        else
                            perror("recv");
                        close(i);
                        FD_CLR(i, &master);
                    }
                    else {
                        printf("Data Recieved: %s\n", buff);
                        for (j = 0 ; j <= fd_max; j++) {
                            FD_ISSET(j, &master);
                            if (j != listener && j != i) {
                                if ((send(j, buff, nbytes, 0) == -1)) {
                                    perror("send");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
