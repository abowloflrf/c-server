//
// Created by ruofeng on 18-4-22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>  //文件打开，加锁等操作
#include <sys/mman.h>
#include <sys/stat.h>   //文件信息
#include <unistd.h>
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

    //TODO:根据URI指定文件名，判断文件是否存在 否则返回404
    //TODO:判断文件累心指定content type
    const char *filename = "/home/ruofeng/www/index.html";
    struct stat sbuf;
    stat(filename, &sbuf);
    int filesize = sbuf.st_size;
    int file_fd = open(filename, O_RDONLY, 0);  //TODO: 打开错误抛出
    char *src_addr = mmap(0, filesize, PROT_READ, MAP_PRIVATE, file_fd, 0);
    memcpy(content, src_addr, filesize);
    close(file_fd);

    //sprintf(content, "<h1>You are visiting: %s</h1>\n<p>have fun</p>", req_hdr->uri);//这里返回URL给客户端
    //size_t len = strlen(content);

    //TODO: 决定返回的ContentType，这里简单的取了请求的第一种
    sprintf(res_hdr, http_res_tmpl, filesize, strsep(&(req_hdr->accept_type), ","));
    size_t len = (size_t) sprintf(res_buf, "%s%s", res_hdr, content);
    return len;
};

void get_filetype(char *filename, char *filetype)
{
    if (strstr(filename, ".html")) {
        strcpy(filetype, "text/html");
    }
    else if (strstr(filename, ".gif")) {
        strcpy(filetype, "image/gif");
    }
    else if (strstr(filename, ".jpg")) {
        strcpy(filetype, "image/jpeg");
    }
    else if (strstr(filename, ".png")) {
        strcpy(filetype, "image/png");
    }
    else {
        strcpy(filetype, "text/plain");
    }
}