/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <errno.h>

#define MY_PORT "3490"
#define BACKLOG 10

void sigchld_handler(int s) {
    (void)s;

    //waitpid() might overwrite errno
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

int main()
{
    struct addrinfo hints, *p, *res;
    int sockfd;

    memset(&hints, 0, sizeof hints);

    // Load up hints
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status;
    if ((status = getaddrinfo(NULL, MY_PORT, &hints, &res)) != 0) {
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

        // Prevent Address already in use error message
        int yes = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("bind");
            continue;
        }

        break;
    }

    // Socket already created, don't need this anymore
    freeaddrinfo(res);

    if (p == NULL) {
        fprintf(stderr, "No available interface for connection\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    // Execute sigchld_handler when a child process terminates from fork()
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while (1) {
        // Listen for clients
        struct sockaddr_storage their_addr;
        socklen_t addr_size = sizeof their_addr;
        int new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &addr_size);

        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        if (!fork()) {
            close(sockfd);
            // Send message to client
            char *message = "Once upon a time...";

            if (send(new_fd, message, strlen(message), 0) == -1) {
                perror("send");
            }
            close(new_fd);
            exit(0);
        }
        close(new_fd);
    }

    return 0;
}