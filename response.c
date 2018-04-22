//
// Created by ruofeng on 18-4-22.
//

#include <stdio.h>
#include <string.h>
#include "request.h"
#include "response.h"

size_t response_handler(char *res_buf, struct http_req_hdr *req_hdr)
{
    char *http_res_tmpl = "HTTP/1.1 200 OK\r\n"
                          "Server: ruofeng's Server V1.0\r\n"
                          "Accept-Ranges: bytes\r\n"
                          "Content-Length: %d\r\n"
                          "Connection: close\r\n"
                          "Content-Type: %s\r\n\r\n";

    char res_hdr[HEADER_BUFF_SIZE], content[BODY_BUFF_SIZE];
    sprintf(content, "<h1>You are visiting: %s</h1>\n<p>have fun</p>", req_hdr->uri);//这里返回URL给客户端
    size_t len = strlen(content);
    //TODO: 决定返回的ContentType，这里简单的取了请求的第一种
    sprintf(res_hdr, http_res_tmpl, len, strsep(&(req_hdr->accept_type), ","));
    len = (size_t) sprintf(res_buf, "%s%s", res_hdr, content);
    return len;
};