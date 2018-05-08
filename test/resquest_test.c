//
// Created by ruofeng on 18-5-8.

#include "../request.h"
#include <stdio.h>
#include <string.h>

int main()
{
    char post_without_data[] =
        "POST /insert.php HTTP/1.1\r\nHost: 127.0.0.1:8888\r\nUser-Agent: curl/7.58.0\r\nAccept: */*\r\n\r\n";
    char post_with_data[] =
        "POST /insert.php HTTP/1.1\r\nHost: 127.0.0.1:8888\r\nUser-Agent: curl/7.58.0\r\nAccept: */*\r\nContent-Length: 20\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\ncontent=123&user=qqq";

    struct http_req_hdr hdr;
    char *req_str = post_without_data;
    request_handler(&hdr, req_str, (int) strlen(post_with_data));
}

