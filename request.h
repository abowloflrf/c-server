//
// Created by ruofeng on 18-4-21.
//

#ifndef C_SERVER_REQUEST_H
#define C_SERVER_REQUEST_H

#define HTTP_METHOD_UNKNOWN         0x0001
#define HTTP_METHOD_GET             0x0002
#define HTTP_METHOD_HEAD            0x0004
#define HTTP_METHOD_POST            0x0008

#define ROOT_PATH "/home/ruofeng/Dev/c-server/dist"         //文档路径

struct http_req_hdr
{
    int method;
    char *host;
    char *uri;
    char *accept_type;
    char req_file[256];     //请求文件完整路径
    char *query_str;    //请求query string，不带问号
    char *content_type;
    unsigned int content_length;
    char *req_body;

};

void request_handler(struct http_req_hdr *, char *, int);
void parse_request_method(struct http_req_hdr *, char *);
void parse_query_string(struct http_req_hdr *);
void parse_request_file(struct http_req_hdr *);

void init_request(struct http_req_hdr *);

#endif //C_SERVER_REQUEST_H
