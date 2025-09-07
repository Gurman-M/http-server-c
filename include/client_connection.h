#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <poll.h>

void del_from_pfds(int *i, struct pollfd **pfds, int *nfds);
void add_to_pfds(int fd, struct pollfd **pfds, int *nfds, int *pfds_size);
void handle_new_connection(int listener, struct pollfd **pfds, int *nfds, int *pfds_size);
int generate_http_response(char *filename, char **dst);
void handle_client_data(int client_fd, int *i, struct pollfd **pfds, int *nfds);
void process_connections(int listener, struct pollfd **pfds, int *nfds, int *pfds_size);

#endif