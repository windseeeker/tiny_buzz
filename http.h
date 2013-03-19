#ifndef _HTTP_H_
#define _HTTP_H_

/* http packet define
   2013-1-25 by shc
*/

#define MAX_URI_PATH_LEN 1024

#define  EGET 0
#define  EPUT 1
#define  EPOST 2
#define  EDELETE 3

#define  GET    "GET"
#define  POST   "POST"
#define  PUT    "PUT"
#define  DELETE "DELETE"

#define DEFAULT_AUTH_SCHEME "Basic"
#define HTTP_VERSION_LEN 8

char *base64_encode(const char*,char **);
char *base64_decode(const char*,char **);

struct Version {
    char high;
    char low;
};
void version_to_str(struct Version ver, char** out_ver);

struct RequestLine {
    char method[8];
    char uri[MAX_URI_PATH_LEN];
    struct Version ver;
};

struct AuthParam {
    char scheme[32];
    char base64[256];
};

struct RequestHeader {
    struct AuthParam auth;
};

struct RequestBody {
};

struct Request {
    struct RequestLine req_line;
    struct RequestHeader req_header;
    struct RequestBody req_body;
};

struct ResponseHeader {
    struct Version ver;
    short status_code;
    char message[512];
};

struct Response {
    struct ResponseHeader resp_header;
};

#endif
