#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "network.h"
#include "file_utils.h"
#include "client_connection.h"


#define INDEX "html/index.html"

/**
 * Close the connection of a client socket and remove it from the array of fds
 * 
 * @param i the index where we must delete an fd
 * @param pfds
 * @param nfds
 */
void del_from_pfds(int *i, struct pollfd **pfds, int *nfds)
{
    close((*pfds)[*i].fd);
    (*pfds)[*i] = (*pfds)[*nfds - 1]; // overwrite closed socket with last fd
    (*nfds)--; // shrink count of open connections
    (*i)--; // so outer loop rechecks swapped fd
}

/**
 * Add the given client fd to the array of fds
 * 
 * @param fd the client fd to add
 * @param pfds array of fds the server is maintaining a connection with
 * @param nfds the number of fds in the array
 * @param pfds_size size of pfds array
 */
void add_to_pfds(int fd, struct pollfd **pfds, int *nfds, int *pfds_size)
{
    if (*nfds == *pfds_size) {
        *pfds_size *= 2;
        *pfds = realloc(*pfds, sizeof **pfds * (*pfds_size));

        if (*pfds == NULL) {
            perror("realloc");
            exit(1);
        }
    }

    (*pfds)[*nfds].fd = fd;
    (*pfds)[*nfds].events = POLLIN;
    (*pfds)[*nfds].revents = 0;
    (*nfds)++;
}

/**
 * Accept and process the new client connection to the server
 * 
 * @param listener the listening socket of the server
 * @param pfds
 * @param nfds
 * @param pfds_size
 */
void handle_new_connection(int listener, struct pollfd **pfds, int *nfds, int *pfds_size) 
{
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof their_addr;
    char remoteIP[INET6_ADDRSTRLEN];

    int new_fd = accept(listener, (struct sockaddr *)&their_addr, &addr_size);

    if (new_fd == -1) {
        perror("accept");
    } else {
        add_to_pfds(new_fd, pfds, nfds, pfds_size);
        printf("server: new connection from %s on socket %d\n", inet_ntop2(&their_addr, remoteIP, sizeof remoteIP), new_fd);
    }
}

/**
 * Generate an HTTP response and serve an HTML file's contents within the body
 * 
 * @param filename
 * @param dst the string to store the result
 * 
 * @return 0 on success, -1 otherwise
 */
int generate_http_response(char *filename, char **dst)
{
    // Create body for HTTP response
    long int file_size = find_file_size(filename);

    if (file_size < 0) {
        return -1;
    }

    char *body = calloc(1, file_size + 1);

    if (body == NULL) {
        fprintf(stderr, "Failed to allocate memory for HTTP response body\n");
        return -1;
    }

    if (read_file_contents(filename, file_size, &body) != 0) {
        return -1;
    }

    char *response = malloc(2048);
    int len = snprintf(response, 2048,
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: %zu\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"  // client will close after response
        "\r\n"
        "%s",
        strlen(body), body
    );

    free(body);

    if (len < 0) {
        perror("snprintf");
        return -1;
    }

    *dst = response;
    return 0;
}

/**
 * Send the client HTML data if the connection is still active
 * 
 * @param client_fd
 * @param i the index in the pfds array where the client_fd is located
 * @param pfds
 * @param nfds
 */
void handle_client_data(int client_fd, int *i, struct pollfd **pfds, int *nfds)
{
    // Check if client connection is still alive
    char reqbuf[2048];
    size_t n = recv(client_fd, reqbuf, sizeof reqbuf, 0);
    if (n <= 0) {
        // 0 => peer closed; -1 => error
        del_from_pfds(i, pfds, nfds);
        return; // no point trying to send anything to this socket
    }
    
    // Parse HTTP data and return the appropriate response
    char *response = NULL;
    if (generate_http_response(INDEX, &response) != 0) {
        fprintf(stderr, "Error generating HTTP response\n");
        return;
    }

    // Send the response
    size_t response_len = strlen(response);
    size_t total_sent = 0;
    while (total_sent < response_len) {
        size_t sent = send(client_fd, response + total_sent, response_len - total_sent, 0);
        if (sent <= 0) { // client error/disconnected
            perror("send");
            break;
        }
        total_sent += sent;
    }

    free(response);
    // CLose the connection to client
    close(client_fd);
    del_from_pfds(i, pfds, nfds);
}

/**
 * Process all active and incoming client connections to the server
 * 
 * @param listener
 * @param pfds
 * @param nfds
 * @param pfds_size
 */
void process_connections(int listener, struct pollfd **pfds, int *nfds, int *pfds_size)
{
    // Go through each fd in pfds and check which event got triggered
    for (int i = 0; i < *nfds; i++) {
        if ((*pfds)[i].revents & (POLLIN | POLLHUP)) {
            if ((*pfds)[i].fd == listener) {
                handle_new_connection(listener, pfds, nfds, pfds_size);
            } else {
                // Client ready to receive data
                handle_client_data((*pfds)[i].fd, &i, pfds, nfds);
            }
        }
    }
}