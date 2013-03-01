#include "http.h"
#include "http_sock.h"
#include "sock.h"

#include <errno.h>
#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BASIC_STR "Basic"

static char _loginname[] = "shc";
static char _passwd[] = "shc";

static char __read_buf[4 * 1024] = {0};
static void copy_to_buf(const char* buf, int buf_len);

static void* recv_data_func(void* arg) {
    int fd = *((int*)arg);
    char buf[1024];
    while (1) {
        int size = read(fd, buf, sizeof(buf));
        if (size == 0) {
            printf("Peer closed socket\n");
            exit(0);
        }
        if (size == -1) {
            if (errno != EAGAIN) {
                printf("socket read data error.fd:%d\n", fd);
                printf("errno:%d\n", errno);
                exit(0);
            }
            sleep(3);
        }
        printf("read %d bytes data\n", size);
        copy_to_buf(buf, size);
    }
}

static void copy_to_buf(const char* buf, int buf_len) {
    static int pos = 0;
    if (pos + buf_len > 4096) {
        printf("Read Buff Already full.\n");
        return;
    }
    memcpy(__read_buf + pos, buf, buf_len);
    pos += buf_len;
    sealize_http_buf(__read_buf, pos);
    /*if (resp) {
        printf("HTTP Server return. version: %s, status_code:%d, "
               "message : %s", resp->version, resp->status_code,
               resp->message);
        free(resp);
    }
    */
}

static
pthread_t create_work_thread(int* fd) {
    pthread_t id;
    pthread_create(&id, NULL, recv_data_func, fd);
    return id;
}

static
void parse_http_command(char* str_to_parse, char* http_method,
                        char* http_params) {
    if (!str_to_parse || ! (*str_to_parse)) {
        printf("Invalid input command str, stop parse\n");
        return;
    }
    if (!http_method || !http_params) {
        printf("Invalid output command recv buf, stop parse\n");
        return;
    }
    int buf_len = strlen(str_to_parse);
    short method_pos = 0, param_start = 0, param_end = buf_len;
    int i = 0;
    for (; i < buf_len; ++i) {
        if (str_to_parse[i] == ' ') {
            if (method_pos == 0) {
                method_pos = i;
            }
            if (i + 1 <= buf_len && str_to_parse[i+1] != ' ') {
                param_start = i + 1;
            }
            continue;
        }
        if (str_to_parse[i] != ' ' && i + 1 <= buf_len
            && (str_to_parse[i+1] == '\r' || str_to_parse[i+1] == 0))
        {
            param_end = i + 1;
            break;
        }
    }

    memcpy(http_method, str_to_parse, method_pos);
    http_method[method_pos] = 0;
    memcpy(http_params, str_to_parse+param_start, param_end - param_start);
    http_params[param_end] = 0;
}

int main(int argc, char**argv) {
    char remote_ip[] = "10.0.203.77";
    short port=8000;

    struct sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(port);
    remote_addr.sin_addr.s_addr = inet_addr(remote_ip);
    struct client_sock* client = create_client_sock(&remote_addr);
    if (!client) {
        printf("Error to create client sock\n");
        return 0;
    }
    char i = 0;
    while (connect_data(client) !=0 && i < 5) {
        sleep(2);
        ++i;
        printf("start reconnect server...\n");
    }
    if (i >= 5) {
        printf("Fail to connect http server\n");
        return 0;
    }
    printf("Success to connect server\n");
    sleep(1);
    pthread_t work_thread = create_work_thread(&client->fd); 

    /*
      Main thread is used to interact with user
    */
    struct RequestLine* line = (struct RequestLine*)
        malloc(sizeof(struct RequestLine));
    line->ver.high = 1;
    line->ver.low = 1;

    struct RequestHeader* header = (struct RequestHeader*)
        malloc(sizeof(struct RequestHeader));

    {
        char* login_info = (char*)malloc(1024);
        sprintf(login_info, "%s:%s", _loginname, _passwd);
        memcpy(header->auth.scheme, BASIC_STR, strlen(BASIC_STR) + 1);
        char* base64 = (char*)malloc(512);
        bzero(base64, 512);
        base64_encode(login_info, &base64);
        memcpy(header->auth.base64, base64, strlen(base64) + 1);
        free(login_info);
        free(base64);
    }

    int send_size;
    char* read_buf = (char*)malloc(1024);
    char* param = (char*)malloc(1024);
    bzero(read_buf, 1024);
    bzero(param, 0);
    char method[8] = {0};
    printf("Please Print command, --help for help\n");
    while (1) {
        fgets(read_buf, 1000, stdin);
        if (strlen(read_buf) <= 1) {
            continue;
        }
        read_buf[strlen(read_buf) - 1] = 0;
        if (!strcmp(read_buf, "--help")) {
            printf ("print http method, then space, last print "
                    "param you want to prase to reqeust\n");
            printf("Please input command : \n");
            continue;
        }

        parse_http_command(read_buf, method, param);
        switch (get_method_from_str(method)) {
            case EGET:
            {
                printf("Sending GET + %s\n", param);
                memcpy(line->method, "GET", 4);
                memcpy(line->uri, param, strlen(param) + 1);
                if (!strncmp(param, "/admin/broad", strlen("/admin/broad"))) {
                    char sub_param[512] = {0};
                    strcpy(sub_param, param + 13);
                    send_size = send_request_header(client, line, header, sub_param);
                    continue;
                }
                send_size = send_request_header(client, line, header, 0);
                if (send_size <= 0) {
                    printf("Error to send data.");
                    return 0;
                }
            }
            break;
            case EPOST:
            {
                printf("Sending POST +  %s\n", param);
                memcpy(line->method, "POST", strlen("POST"));
                memcpy(line->uri, param, strlen(param) + 1);
                send_size = send_request_header(client, line, header, 0);
            }
            break;
            case EPUT:
            {
                printf("Sending PUT +  %s\n", param);
                memcpy(line->method, "PUT", strlen("PUT"));
                memcpy(line->uri, param, strlen(param) + 1);
                send_size = send_request_header(client, line, header, 0);
            }
            break;
            case EDELETE:
            {
                printf("Sending DELETE +  %s\n", param);
                memcpy(line->method, "DELETE", strlen("DELETE"));
                memcpy(line->uri, param, strlen(param) + 1);
                send_size = send_request_header(client, line, header, 0);

            }
            break;
            default:
            {
                printf("Error method type, quit\n");
            }
            sleep(1);
            printf("Please input command : \n");
        }
        printf("Already sent data : %d\n", send_size);
    }
    return 0;
}
