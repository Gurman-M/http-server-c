/*
** tl1.c -- a datagram socket talker/listener demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define SEND_PORT "3490"
#define LISTEN_PORT "4950"
#define MAXDATASIZE 100
#define TYPEWRITER_DELAY 50000

void typewriter(const char *text, useconds_t delay) {
    for (const char *p = text; *p; p++) {
        putchar(*p);
        fflush(stdout);
        usleep(delay);
    }
    printf("\n");
}

void send_message(int sockfd, char *message, struct addrinfo *p) {
    int numbytes;
    if ((numbytes = sendto(sockfd, message, strlen(message), 0, p->ai_addr, p->ai_addrlen)) == -1) {
        perror("sendto");
        exit(1);
    }

    // printf("tl1: sent message '%s'\n", message);
    sleep(2);
}

void recv_message(int sockfd) {
    struct sockaddr_storage their_addr;
    socklen_t fromlen = sizeof their_addr;
    char message[MAXDATASIZE];
    int numbytes;
    if ((numbytes = recvfrom(sockfd, message, sizeof message, 0, (struct sockaddr *)&their_addr, &fromlen)) == -1) {
        perror("recv");
        return;
    }

    message[numbytes] = '\0';

    typewriter(message, TYPEWRITER_DELAY);
    sleep(1);
}

int create_socket(char *addr, char *port, int listen, struct addrinfo **curr_iface, struct addrinfo **iface_list) {
    struct addrinfo hints, *p, *res;
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    if (listen) {
        hints.ai_flags = AI_PASSIVE;
    }

    int status;
    if ((status = getaddrinfo(addr, port, &hints, &res)) != 0) {
        fprintf(stderr, "Error: %s\n", gai_strerror(status));
        return -1;
    }

    // Loop through getaddrinfo return
    for (p = res; p != NULL; p = p->ai_next) {
        // Try creating a socket, if it fails move on to next result
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        if (listen && bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "No available interface for connection\n");
        return -1;
    }

    *curr_iface = p;
    *iface_list = res;

    return sockfd;
}

int main()
{
    struct addrinfo *p, *res;

    int listensockfd = create_socket(NULL, LISTEN_PORT, 1, &p, &res);

    if (listensockfd == -1) {
        fprintf(stderr, "Error creating listen socket\n");
        exit(1);
    }

    // Socket already created, don't need this anymore
    freeaddrinfo(res);

    int sendsockfd = create_socket("127.0.0.1", SEND_PORT, 0, &p, &res);

    if (sendsockfd == -1) {
        fprintf(stderr, "Error creating send socket\n");
        exit(1);
    }

    send_message(sendsockfd, "Ahem! Today...", p);
    recv_message(listensockfd);
    send_message(sendsockfd, "This prophetic book was a mysterious tome full of stories of future events.", p);
    recv_message(listensockfd);
    send_message(sendsockfd, "But no person, after obtaining this amazing book, ever found happiness.", p);
    recv_message(listensockfd);
    send_message(sendsockfd, "That book came to be called the Dark Prognosticus and was sealed away.", p);
    recv_message(listensockfd);
    send_message(sendsockfd, "It is a tale of love...", p);

    // Socket already created, don't need this anymore
    freeaddrinfo(res);

    close(sendsockfd);
    close(listensockfd);

    return 0;
}