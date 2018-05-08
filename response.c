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

    if (req_hdr->uri[strlen(req_hdr->uri) - 1] == '/') {
        strcat(filename, "index.html");             //默认文档index.html
    }
    if (stat(filename, &sbuf) != 0) {               //文件打开失败返回404
        send_error_response(socket_fd, "404", "Not Found");
        return;
    }
    else {
        get_filetype(filename, filetype);           //获取文件类型
        size_t filesize = (size_t) sbuf.st_size;    //获取文件大小
        int file_fd = open(filename, O_RDONLY, 0);
        if (file_fd == -1) {                        //文件打开失败返回404
            send_error_response(socket_fd, "404", "Not Found");
            return;
        }
        char *src_addr = mmap(0, filesize, PROT_READ, MAP_PRIVATE, file_fd, 0);
        if (src_addr == MAP_FAILED) {               //文件映射失败返回404
            send_error_response(socket_fd, "404", "Not Found");
            return;
        }
        close(file_fd);                                                         //关闭文件流
        if (strcmp("script/php", filetype) == 0) {
            int fpm_fd = open_fpm_sock();                                       //打开php-fpm socket
            send_fastcgi(&rio, req_hdr, fpm_fd);                                //解析请求到FastCGI标准并向socket发送
            recv_fastcgi(send_to_c, rio.rio_fd, fpm_fd);                        //从PHP接受解释完成的输出
            close(fpm_fd);                                                      //关闭php-fpm socket
        }
        else {
            sprintf(res_hdr, http_res_tmpl, "200 OK", filesize, filetype);      //组合头部
            rio_writen(socket_fd, res_hdr, strlen(res_hdr));                    //写入头部
            rio_writen(socket_fd, src_addr, filesize);                          //写入body
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
    //200 "Content-type: text/html; charset=UTF-8\r\n\r\nhello from phper/dist"
    //302 "Status: 302 Found\r\nLocation: /\r\nContent-type: text/html; charset=UTF-8\r\n\r\n"
    //500 "Status: 500 Internal Server Error\r\nContent-type: text/html; charset=UTF-8\r\n\r\n"
    char *http_res_tmpl = "HTTP/1.1 %s %s\r\n"
                          "Server: ruofeng's Server\r\n"
                          "Content-Length: %d\r\n"
                          "Content-Type: %s\r\n\r\n";

    if (strncmp(out, "Status:", 7) == 0) {
        char status[3];             //保存状态码
        char status_str[30];        //保存状态说明字符串
        char response_type[50];     //保存内容类型
        char *line = strsep(&out, "\r\n");
        out++;
        while (line != NULL) {
            //直接跳出不再解析响应头部
            if (strncmp(line, "\0", 1) == 0)
                break;
            //解析状态码与状态说明字符串
            if (strncmp(line, "Status:", 7) == 0) {
                strncpy(status, line + 8, 3);
                strcpy(status_str, line + 12);
            }
            if (strncmp(line, "Content-type:", 13) == 0) {
                strcpy(response_type, line + 14);
            }
            line = strsep(&out, "\r\n");
            out++;
        }
        sprintf(buf, http_res_tmpl, status, status_str, errlen, response_type);
        rio_writen(fd, buf, strlen(buf));
        rio_writen(fd, err, errlen);
        return errlen;
    }

    //开头未输出状态码则默认为200
    p = index(out, '\r');
    n = (int) (p - out);
    sprintf(buf, http_res_tmpl, "200", "OK", outlen - n - 4, "text/html");
    rio_writen(fd, buf, strlen(buf));
    rio_writen(fd, p + 4, outlen - n - 4);
    //FIXME: 输出过长内容时，如phpinfo，会出错
    return outlen;
}