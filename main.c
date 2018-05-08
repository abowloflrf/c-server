//
// Created by ruofeng on 18-4-20.
//
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h> // socket
#include <sys/types.h>  // 基本数据类型
#include <arpa/inet.h>
#include <unistd.h> // read write
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "request.h"
#include "response.h"
#include "log.h"

#define SERV "0.0.0.0"
#define QUEUE 20
#define BUFF_SIZE 8192

int sockfd;

void handle_signal(int sign); // 退出信号处理
void http_send(int, struct http_req_hdr *); // http 发送相应报文

int main(int argc, char *argv[])
{
    //TODO: 从config file中读取配置
    uint16_t port = 8888;
    if (argc > 1) {
        port = (uint16_t) atoi(argv[1]);
    }

    signal(SIGINT, handle_signal);  //处理中断退出信号

    // 定义 socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 定义 sockaddr_in
    struct sockaddr_in skaddr;
    skaddr.sin_family = AF_INET;                //ipv4
    skaddr.sin_port = htons((uint16_t) port);   //8888
    skaddr.sin_addr.s_addr = inet_addr(SERV);   //0.0.0.0

    int tr = 1;
    // 防止出现 "Address already in use" ，取消关闭socket时端口保留等待时间
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    // bind，绑定 socket 和 sockaddr_in
    if (bind(sockfd, (struct sockaddr *) &skaddr, sizeof(skaddr)) == -1) {
        perror("bind error");
        exit(1);
    }

    // listen，开始添加端口
    if (listen(sockfd, QUEUE) == -1) {
        perror("listen error");
        exit(1);
    }

    // 客户端信息
    char buff[BUFF_SIZE];
    struct sockaddr_in claddr;
    socklen_t length = sizeof(claddr);


    while (1) {
        int sock_client = accept(sockfd, (struct sockaddr *) &claddr, &length); //accept，开始接受传入的连接
        if (sock_client < 0) {
            perror("accept error");
            exit(1);
        }
        int len = (int) recv(sock_client, buff, sizeof(buff), 0);

        struct http_req_hdr req_hdr;                //新建一个请求头部结构体
        request_handler(&req_hdr, buff, len);       //request_handler解析请求头部并返回一个简单的请求结构体
        http_send(sock_client, &req_hdr);           //发送HTTP response
        close(sock_client);                         //response发送完毕，关闭客户端连接
    }
}

void http_send(int sock_client, struct http_req_hdr *req_hdr)
{
    log_request(req_hdr);
    response_handler(sock_client, req_hdr);
}

void handle_signal(int sign)
{
    fprintf(stdout, "\nRecieved signal: %d\nBye~\n", sign);
    shutdown(sockfd, SHUT_RDWR);
    exit(0);
}