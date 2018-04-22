//
// Created by ruofeng on 18-4-21.
//

#ifndef C_SERVER_REQUEST_H
#define C_SERVER_REQUEST_H

#define HTTP_METHOD_UNKNOWN         0x0001
#define HTTP_METHOD_GET             0x0002
#define HTTP_METHOD_HEAD            0x0004
#define HTTP_METHOD_POST            0x0008


struct http_req_hdr
{
    int method;
    char *host;
    char *uri;
    char *accept_type;
};

struct http_req_hdr *request_handler(char *, int);
void parse_request_method(struct http_req_hdr *header, char *line);
#endif //C_SERVER_REQUEST_H
