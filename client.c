////
////  client_send.c
////
////  client for sending to select server
////  Created by Shantanu Jha on 8/4/16.
////  Copyright © 2016 BlueBall. All rights reserved.


#include "servient.h"
#define MY_PORT "6969"
const int BUFF_SIZE = 256;

void* get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    else
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(int argc, char** argv) {
    if (argc < 3) {
        printf("usage: ./send IP s/r\n");
        printf("P.S. if using s, give a message after that\n");
        exit(1);
    }
    
    if (argc <= 3 && (strcmp(argv[2], "s") == 0)) {
        printf("usage: ./send IP s message\n");
        exit(1);
    }
    
    int status;
    int sock_fd;
    int n_bytes;
    char buff[BUFF_SIZE];
    struct addrinfo info, *p, *ai;
    char* mssg = (char*) malloc(BUFF_SIZE);
    char s[INET_ADDRLEN6];
    int size;
    
    int i;
    
    for (i = 3; i < argc-1; i++) {
        mssg = strcat(mssg, argv[i]);
        mssg = strcat(mssg," ");
    }
    mssg = strcat(mssg, argv[argc-1]);
    size = strlen(mssg);
    
    assert(strlen(mssg) < (BUFF_SIZE -1));

    
    memset(&info, 0, sizeof(info));
    
    info.ai_family = AF_UNSPEC;
    info.ai_socktype = SOCK_STREAM;
    
    status = getaddrinfo(argv[1], MY_PORT, &info, &ai);
    
    if (status < 0) {
        fprintf(stderr, "getaddrinfo: %s", gai_strerror(status));
        exit(1);
    }
    
    for (p = ai; p != NULL; p = p->ai_next) {
        sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        
        if (sock_fd == -1) {
            perror("socket");
            continue;
        }
        
        if ((connect(sock_fd, (struct sockaddr*)p->ai_addr, p->ai_addrlen)) == -1) {
            close(sock_fd);
            perror("connect");
            continue;
        }
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        exit(2);
    }
    
    freeaddrinfo(ai);
    
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), s, INET_ADDRLEN6);
    printf("client connecting to %s\n", s);
    
    if (strcmp(argv[2], "s") == 0) {
    
        if ((send(sock_fd, mssg, size, 0)) == -1) {
            //close(sock_fd);
            perror("send");
            exit(1);
        }
        //close(sock_fd);
    }
    
    else if (strcmp(argv[2], "r") == 0) {
        printf("ready to receive\n");
        if ((n_bytes = recv(sock_fd, buff, BUFF_SIZE - 1, 0)) <= 0) {
            if (n_bytes == 0) {
                printf("connection lost because server hung up...\n");
                exit(1);
            }
            else {
                perror("recv");
                exit(1);
            }
        }
        
        printf("Message: %s\n", buff);
        //close(sock_fd);
    }
    
    close(sock_fd);
    return 0;
}