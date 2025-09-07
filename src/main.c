/*
** main.c -- a lightweight HTTP server
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <poll.h>

#include "network.h"
#include "client_connection.h"
#include "file_utils.h"

void handle_sigint(int sig)
{
    exit(0);
}

int main(void)
{
    signal(SIGINT, handle_sigint);
    
    int nfds = 0;
    int pfds_size = 2;
    struct pollfd *pfds = malloc(sizeof *pfds * pfds_size);

    int listener = get_listener_socket();
    if (listener == -1) {
        fprintf(stderr, "Failed to create listener socket\n");
        exit(1);
    }

    pfds[0].fd = listener;
    pfds[0].events = POLLIN;
    nfds++;

    puts("server: waiting for connections...");

    while (1) {
        int rv = poll(pfds, nfds, -1);

        if (rv == -1) {
            perror("poll");
            free(pfds);
            exit(1);
        }

        process_connections(listener, &pfds, &nfds, &pfds_size);
    }

    free(pfds);
    return 0;
}