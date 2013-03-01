#include "http.h"
#include "http_sock.h"
#include "sock.h"

#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

int get_method_from_str(const char* method) {
    if (!method || !(*method)) {
        return -1;
    }
    if (!strcmp(method, GET)) {
        return EGET;
    }
    if (!strcmp(method, POST)) {
        return EPOST;
    }
    if (!strcmp(method, PUT)) {
        return EPUT;
    }
    if (!strcmp(method, DELETE)) {
        return EDELETE;
    }
    return -1;
}

int send_http_request(struct client_sock* client, struct RequestLine* line, struct RequestHeader* header, const char* body) {
    if (!line) {
        return -1;
    }
    char buf_send[2048] = {0};
    sprintf(buf_send, "%s %s HTTP/%d.%d", line->method, line->uri,line->ver.high, line->ver.low);
    buf_send[strlen(buf_send)] = 0x0A;
    if (header) {
        sprintf(buf_send, "%sAuthorization: %s %s", buf_send, header->auth.scheme, header->auth.base64);
        buf_send[strlen(buf_send)] = 0x0A;
    }
    strcat(buf_send, "Host: 8080");
    buf_send[strlen(buf_send)] = 0x0A;
    if (!body) {
        buf_send[strlen(buf_send)] = 0x0D;
        buf_send[strlen(buf_send)] = 0x0A;
        return send_data(client, buf_send, strlen(buf_send));
    }
    return 0;
}

int send_request_header(struct client_sock* client, struct RequestLine* line,
                        struct RequestHeader* header, char* body) {
    if (!line) {
        return -1;
    }
    char buf_send[2048] = {0};
    sprintf(buf_send, "%s %s HTTP/%d.%d", line->method, line->uri,
            line->ver.high, line->ver.low);
    buf_send[strlen(buf_send)] = 0x0A;
    if (header) {
        sprintf(buf_send, "%sAuthorization: %s %s", buf_send,
                header->auth.scheme, header->auth.base64);
        buf_send[strlen(buf_send)] = 0x0A;
    }
    strcat(buf_send, "Host: 8080");
    buf_send[strlen(buf_send)] = 0x0A;
    buf_send[strlen(buf_send)] = 0x0D;//null line
    if (body) {
        strcat(buf_send, body);
    }
    buf_send[strlen(buf_send)] = 0x0A;
    return send_data(client, buf_send, strlen(buf_send));
}

int send_post_request(struct client_sock* client, struct RequestLine* line,
                      struct RequestHeader* header, char* body) {
    return send_request_header(client, line, header, body);
}

int send_put_request(struct client_sock* client, struct RequestLine* line,
                     struct RequestHeader* header, char* body) {
    return 0;
}

int send_delete_request(struct client_sock* client, struct RequestLine* line,
                        struct RequestHeader* header, char* body) {
    return 0;
}

void sealize_http_buf(const char* buf, int buf_pos) {
    static int read_pos = 0;
    char version[HTTP_VERSION_LEN + 1] = {0};
    if (buf_pos < HTTP_VERSION_LEN) {
        return;
    }
    memcpy(version, buf + read_pos, HTTP_VERSION_LEN);
    read_pos += HTTP_VERSION_LEN;
    buf_pos -= HTTP_VERSION_LEN;
    ++read_pos; /* 1 space offset */
    ++buf_pos;
    printf("http_version: %s\n", version);

    if (buf_pos > sizeof(int)) {
        int status_code;
        memcpy(&status_code, buf + read_pos, sizeof(status_code));
        printf("error_status:%d\n", status_code);
    }
    read_pos += sizeof(short);
    buf_pos -= sizeof(short);
    ++read_pos;
    ++buf_pos;
    char message[512] = {0};
    if (buf_pos > 0) {
        memcpy(message, buf + read_pos, 512);
        printf("message:%s\n", message);
    }
    return;
}
