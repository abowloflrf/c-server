//
// Created by ruofeng on 18-4-21.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "request.h"


void request_handler(struct http_req_hdr *header,char *request_header, int len)
{

    header->method = HTTP_METHOD_UNKNOWN;
    header->accept_type = "text/plain";
    header->host = "unkownlhost";
    header->uri = "/";

    //按照\r\n分割解析HTTP头部
    char *line = strsep(&request_header, "\r\n");
    int index = 0;
    while (line != NULL) {
        if (index == 0) {
            parse_request_method(header, line);
            goto spilt;
        }
        //Host: Accpet:
        char *value = strsep(&line, " ");

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

        spilt:
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