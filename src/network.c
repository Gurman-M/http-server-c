#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#include "network.h"

#define MY_PORT "3490"
#define BACKLOG 10

/**
 * Prints the IP address of the given address struct
 *
 * @param addr the socket for which we will print the address (either IPv4 or IPv6)
 * @param buf the destination buffer to write the IP address
 * @param size size of destination buffer
 */
const char *inet_ntop2(void *addr, char *buf, size_t size) {
    struct sockaddr_storage *sas = addr;
    struct sockaddr_in *sa4;
    struct sockaddr_in6 *sa6;
    void *src;

    switch (sas->ss_family) {
        case AF_INET:
            sa4 = addr;
            src = &(sa4->sin_addr);
            break;
        case AF_INET6:
            sa6 = addr;
            src = &(sa6->sin6_addr);
            break;
        default:
            return NULL;
    }
    
    return inet_ntop(sas->ss_family, src, buf, size);
}

/**
 * Creates a socket that binds and listens on the port defined as MY_PORT
 *
 * @return the socket fd if successful, otherwise -1
 */
int get_listener_socket(void) {
    struct addrinfo hints, *p, *res;
    int sockfd;

    memset(&hints, 0, sizeof hints);

    // Load up hints
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status;
    if ((status = getaddrinfo(NULL, MY_PORT, &hints, &res)) != 0)
    {
        fprintf(stderr, "Error: %s\n", gai_strerror(status));
        return -1;
    }

    // Loop through getaddrinfo return
    for (p = res; p != NULL; p = p->ai_next)
    {
        // Try creating a socket, if it fails move on to next result
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("socket");
            continue;
        }

        // Prevent Address already in use error message
        int yes = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
        {
            perror("setsockopt");
            return -1;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("bind");
            continue;
        }

        break;
    }

    // Socket already created, don't need this anymore
    freeaddrinfo(res);

    if (p == NULL)
    {
        fprintf(stderr, "No available interface for connection\n");
        return -1;
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        return -1;
    }

    return sockfd;
}