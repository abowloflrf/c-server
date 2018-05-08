//
// Created by ruofeng on 18-4-21.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "request.h"


void request_handler(struct http_req_hdr *header, char *request_header, int len)
{

    header->method = HTTP_METHOD_UNKNOWN;
    header->accept_type = "text/plain";
    header->uri = "/";

    //按照\r\n分割解析HTTP头部
    char *line = strsep(&request_header, "\r\n");
    int index = 0;
    while (line != NULL) {
        if (index == 0) {
            parse_request_method(header, line);     //解析请求方法
            parse_query_string(header);             //解析QueryString
            parse_request_file(header);             //解析请求文件（去除QueryString后内容）
            goto spilt;
        }
        char *value = strsep(&line, " ");

        //Host: Accpet:
        if (strncmp(value, "Host:", 5) == 0) {
            value = strsep(&line, " ");
            header->host = value;
            goto spilt;
        }

        if (strncmp(value, "Accept:", 7) == 0) {
            value = strsep(&line, " ");
            header->accept_type = value;
            goto spilt;
        }

        if (strncmp(value, "Content-Length:", 15) == 0) {
            value = strsep(&line, " ");
            header->content_length = (unsigned int) atoi(value);
            goto spilt;
        }

        if (strncmp(value, "Content-Type:", 13) == 0) {
            value = strsep(&line, " ");
            header->content_type = value;
            goto spilt;
        }

        spilt:
        if (strncmp(request_header, "\n\r\n", 3) == 0) {
            request_header += 3;
            line = request_header;
            header->req_body = line;
            break;
        }
        line = strsep(&request_header, "\r\n");
        index++;
    }
    //TODO: post body的解析
};

void parse_request_method(struct http_req_hdr *header, char *line)
{
    char *value = strsep(&line, " ");
    switch (value[0]) {
    case 'G':header->method = HTTP_METHOD_GET;
        break;
    case 'P':header->method = HTTP_METHOD_POST;
        break;
    case 'H':header->method = HTTP_METHOD_HEAD;
        break;
    default:header->method = HTTP_METHOD_UNKNOWN;
        break;
    }
    value = strsep(&line, " ");
    header->uri = value;
}

void parse_query_string(struct http_req_hdr *header)
{
    char *p = index(header->uri, '?');
    if (p != 0) {
        p++;
        header->query_str = p;
    }
}

void parse_request_file(struct http_req_hdr *header)
{
    header->req_file = (char *) malloc(sizeof(char));
    char path_buf[256];
    sprintf(path_buf, "%s%s", ROOT_PATH, header->uri);
    char *p = index(path_buf, '?');
    if (p)
        strncpy(header->req_file, path_buf, (size_t) (p - path_buf));   //有query string
    else
        strcpy(header->req_file, path_buf);                             //无 query string
}
