/*
** listener2.c -- a datagram socket listener demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT "4950"
#define MAXDATASIZE 100

int main()
{
    struct addrinfo hints, *p, *res;
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    int status;
    if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "Error: %s\n", gai_strerror(status));
        exit(1);
    }

    // Loop through getaddrinfo return
    for (p = res; p != NULL; p = p->ai_next) {
        // Try creating a socket, if it fails move on to next result
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "No available interface for connection\n");
        exit(1);
    }

    // Socket already created, don't need this anymore
    freeaddrinfo(res);

    char message[MAXDATASIZE];
    int numbytes;
    if ((numbytes = recv(sockfd, message, sizeof message, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    message[numbytes] = '\0';

    printf("listener: received '%s'\n", message);

    close(sockfd);

    return 0;
}