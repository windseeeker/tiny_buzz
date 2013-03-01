#include "sock.h"

#include <malloc.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

struct client_sock* create_client_sock(struct sockaddr_in* remote) {
    struct client_sock* sock = (struct client_sock*)malloc(sizeof(struct client_sock));
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        printf("Error to create stream socket\n");
        free(sock);
        return NULL;
    }
    printf("New socket : %d\n", fd);
    int flags = fcntl(fd, F_GETFL, 0);
    //fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    sock->fd = fd;
    sock->remote_addr = *remote;
    return sock;
}

int connect_data(struct client_sock* client) {
    if (client)
        return connect(client->fd, (struct sockaddr*)&(client->remote_addr), sizeof(client->remote_addr));
}

int send_data(struct client_sock* client, char* data, int len) {
    int sent = write(client->fd, data, len);
    return sent;
}
