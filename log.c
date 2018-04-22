//
// Created by ruofeng on 18-4-22.
//
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "request.h"

void log_request(struct http_req_hdr *header)
{
    char method_str[10];
    switch (header->method) {
    case HTTP_METHOD_GET:strcpy(method_str, "GET");
        break;
    case HTTP_METHOD_POST:strcpy(method_str, "POST");
        break;
    case HTTP_METHOD_HEAD:strcpy(method_str, "HEAD");
        break;
    default:strcpy(method_str, "UNKOWN");
        break;
    }
    fprintf(stdout, "%s:%s\n", method_str, header->uri);
}
