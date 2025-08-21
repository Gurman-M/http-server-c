/*
** talker.c -- a datagram socket talker demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define PORT "4950"

int main()
{
    struct addrinfo hints, *p, *res;
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    int status;
    if ((status = getaddrinfo("127.0.0.1", PORT, &hints, &res)) != 0) {
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

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "No available interface for connection\n");
        exit(1);
    }

    // Socket already created, don't need this anymore
    freeaddrinfo(res);

    char *message = "Once upon a time...";
    int numbytes;
    if ((numbytes = sendto(sockfd, message, strlen(message), 0, p->ai_addr, p->ai_addrlen)) == -1) {
        perror("sendto");
        exit(1);
    }

    printf("talker: sent message '%s'\n", message);

    close(sockfd);

    return 0;
}