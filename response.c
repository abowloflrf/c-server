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
#include "request.h"
#include "response.h"

void response_handler(int socket_fd, struct http_req_hdr *req_hdr)
{
    char *http_res_tmpl = "HTTP/1.1 %s\r\n"
                          "Server: ruofeng's Server V1.0\r\n"
                          "Accept-Ranges: bytes\r\n"
                          "Content-Length: %d\r\n"
                          "Content-Type: %s\r\n\r\n";

    char res_hdr[HEADER_BUFF_SIZE];
    char filetype[20];
    char filename[256] = "../www";  //HTML文档指定目录
    strcat(filename, req_hdr->uri); //根据URI拼接请求文档路径
    if (filename[strlen(filename) - 1] == '/') {
        strcat(filename, "index.html"); //默认文档
    }
    struct stat sbuf;
    //文件打开失败返回404
    if (stat(filename, &sbuf) != 0) {
        send_error_response(socket_fd, "404", "Not Found");
    }
    else {
        size_t filesize = (size_t) sbuf.st_size;    //获取文件大小
        int file_fd = open(filename, O_RDONLY, 0);  //TODO: 打开错误抛出
        char *src_addr = mmap(0, filesize, PROT_READ, MAP_PRIVATE, file_fd, 0);
        close(file_fd);//关闭文件流
        //TODO:判断文件类型指定content type
        get_filetype(filename, filetype);
        sprintf(res_hdr, http_res_tmpl, "200 OK", filesize, filetype);

        write_to_socket(socket_fd, res_hdr, strlen(res_hdr));   //写入头部
        write_to_socket(socket_fd, src_addr, filesize);         //写入body
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
    else {
        strcpy(filetype, "text/plain");
    }
}

void send_error_response(int socket_fd, char *status, char *msg)
{
    char *http_res_tmpl = "HTTP/1.1 %s %s\r\n"
                          "Server: ruofeng's Server V1.0\r\n"
                          "Accept-Ranges: bytes\r\n"
                          "Content-Length: %d\r\n"
                          "Content-Type: %s\r\n\r\n";
    char buf[BUFSIZ], body[BUFSIZ];
    sprintf(body, "%s - %s", status, msg);
    sprintf(buf, http_res_tmpl, status, msg, strlen(body), "text/plain");
    strcat(buf, body);
    write_to_socket(socket_fd, buf, strlen(buf));
    //write(socket_fd, buf, strlen(buf));
}

ssize_t write_to_socket(int fd, void *buf, size_t n)
{
    size_t nleft = n; // 剩下的未写入字节数
    ssize_t nwritten;
    char *bufp = (char *) buf;

    while (nleft > 0) {
        if ((nwritten = write(fd, bufp, nleft)) <= 0) {
            if (errno == EINTR) { // 被信号处理函数中断返回
                nwritten = 0;
            }
            else { // write函数出错
                return -1;
            }
        }
        nleft -= nwritten;
        bufp += nwritten;
    }
    return n;
}