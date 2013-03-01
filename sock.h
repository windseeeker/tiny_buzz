#ifndef _SOCKET_H
#define _SOCKET_H

#include <netinet/in.h>

struct client_sock {
    int fd;
    short status;
    struct sockaddr_in remote_addr;
};

struct client_sock* create_client_sock(struct sockaddr_in* remote);
int connect_web(struct client_sock* client);
int send_data(struct client_sock* client, char* data, int len);


#endif
