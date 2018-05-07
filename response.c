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
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "request.h"
#include "response.h"
#include "log.h"
#include "fcgi.h"
#include "io.h"

void response_handler(int socket_fd, struct http_req_hdr *req_hdr)
{
    char *http_res_tmpl = "HTTP/1.1 %s\r\n"
                          "Server: ruofeng's Server\r\n"
                          "Accept-Ranges: bytes\r\n"
                          "Content-Length: %d\r\n"
                          "Content-Type: %s\r\n\r\n";
    char res_hdr[HEADER_BUFF_SIZE];
    char filetype[20];
    char *filename = req_hdr->req_file;

    struct stat sbuf;
    rio_t rio;
    rio_readinitb(&rio, socket_fd);

    //strcat(filename, req_hdr->uri);
    if (req_hdr->uri[strlen(req_hdr->uri) - 1] == '/') {
        strcat(filename, "index.html");     //默认文档index.html
    }
    if (stat(filename, &sbuf) != 0) {       //文件打开失败返回404
        send_error_response(socket_fd, "404", "Not Found");
        return;
    }
    else {
        get_filetype(filename, filetype);           //获取文件类型
        size_t filesize = (size_t) sbuf.st_size;    //获取文件大小
        int file_fd = open(filename, O_RDONLY, 0);
        if (file_fd == -1) {            //文件打开失败返回404
            send_error_response(socket_fd, "404", "Not Found");
            return;
        }
        char *src_addr = mmap(0, filesize, PROT_READ, MAP_PRIVATE, file_fd, 0);
        if (src_addr == MAP_FAILED) {            //文件映射失败返回404
            send_error_response(socket_fd, "404", "Not Found");
            return;
        }
        close(file_fd);                                                 //关闭文件流
        if (strcmp("script/php", filetype) == 0) {
            int fpm_fd = open_fpm_sock();
            send_fastcgi(&rio, req_hdr, fpm_fd);
            recv_fastcgi(send_to_c, rio.rio_fd, fpm_fd);
            close(fpm_fd);
        }
        else {
            sprintf(res_hdr, http_res_tmpl, "200 OK", filesize, filetype);  //组合头部
            rio_writen(socket_fd, res_hdr, strlen(res_hdr));                    //写入头部
            rio_writen(socket_fd, src_addr, filesize);                        //写入body
        }
    }

}

void get_filetype(char *filename, char *filetype)
{
    if (strstr(filename, ".html")) {
        strcpy(filetype, "text/html");
    }
    else if (strstr(filename, ".js")) {
        strcpy(filetype, "application/javascript");
    }
    else if (strstr(filename, ".css")) {
        strcpy(filetype, "text/css");
    }
    else if (strstr(filename, ".gif")) {
        strcpy(filetype, "image/gif");
    }
    else if (strstr(filename, ".jpg") || strstr(filename, ".jpeg")) {
        strcpy(filetype, "image/jpeg");
    }
    else if (strstr(filename, ".png")) {
        strcpy(filetype, "image/png");
    }
    else if (strstr(filename, ".svg")) {
        strcpy(filetype, "image/svg+xml");
    }
    else if (strstr(filename, ".php")) {
        strcpy(filetype, "script/php");
    }
    else {
        strcpy(filetype, "text/plain");
    }
}

void send_error_response(int socket_fd, char *status, char *msg)
{
    char *http_res_tmpl = "HTTP/1.1 %s %s\r\n"
                          "Server: ruofeng's Server\r\n"
                          "Accept-Ranges: bytes\r\n"
                          "Content-Length: %d\r\n"
                          "Content-Type: %s\r\n\r\n";
    char buf[BUFSIZ], body[BUFSIZ];
    sprintf(body, "%s - %s", status, msg);
    sprintf(buf, http_res_tmpl, status, msg, strlen(body), "text/plain");
    strcat(buf, body);
    rio_writen(socket_fd, buf, strlen(buf));
}

ssize_t send_to_c(int fd, size_t outlen, char *out, size_t errlen, char *err, FCGI_EndRequestBody *endr)
{
    char *p;
    int n;
    char buf[BUFSIZ];
    //TODO: 根据out定义不同的response header
    char *http_res_tmpl = "HTTP/1.1 %s %s\r\n"
                          "Server: ruofeng's Server\r\n"
                          "Content-Length: %d\r\n"
                          "Content-Type: %s\r\n\r\n";

    if (errlen == 0) {
        p = index(out, '\r');
        n = (int) (p - out);
        sprintf(buf, http_res_tmpl, "200", "OK", outlen - n - 4, "text/html");
        strncat(buf, p + 4, outlen - n - 4);
        rio_writen(fd, buf, strlen(buf));
        return strlen(buf);
    }
    else {
        sprintf(buf, http_res_tmpl, "500", "Internal Server Error", errlen, "text/plain");
        rio_writen(fd, buf, strlen(buf));
        rio_writen(fd, err, (size_t) errlen);
        return errlen;
    }
}