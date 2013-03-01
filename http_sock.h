#ifndef _HTTP_SOCK_H
#define _HTTP_SOCK_H


/*
  http_associated sock method
  2013-1-25 by shc
*/

struct client_sock;

int get_method_from_str(const char* method);

int send_request_header(struct client_sock* client, struct RequestLine* line,
                        struct RequestHeader* header, char* body);
int send_post_request(struct client_sock* client, struct RequestLine* line,
                      struct RequestHeader* header, char* body);
int send_put_request(struct client_sock* client, struct RequestLine* line,
                     struct RequestHeader* header, char* body);
int send_delete_request(struct client_sock* client, struct RequestLine* line,
                        struct RequestHeader* header, char* body);

void sealize_http_buf(const char* buf, int buf_pos);

#endif
