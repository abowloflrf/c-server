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

#define SERV "0.0.0.0"
#define QUEUE 20
#define BUFF_SIZE 1024


typedef struct doc_type
{
    char *key;
    char *value;
} HTTP_CONTENT_TYPE;

HTTP_CONTENT_TYPE http_content_type[] = {
    {"html", "text/html"},
    {"gif", "image/gif"},
    {"jpeg", "image/jpeg"}
};

int sockfd;

char *http_res_tmpl = "HTTP/1.1 200 OK\r\n"
                      "Server: ruofeng's Server V1.0\r\n"
                      "Accept-Ranges: bytes\r\n"
                      "Content-Length: %d\r\n"
                      "Connection: close\r\n"
                      "Content-Type: %s\r\n\r\n";

void handle_signal(int sign); // 退出信号处理
void http_send(int sock_client, char *content); // http 发送相应报文

int main(int argc, char *argv[])
{
    //TODO: 从config file中读取配置
    int port = 8888;
    if (argc > 1) {
        port = atoi(argv[1]);//TODO: uint16
    }

    signal(SIGINT, handle_signal);  //处理中断退出信号

    // 定义 socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 定义 sockaddr_in
    struct sockaddr_in skaddr;
    skaddr.sin_family = AF_INET;                //ipv4
    skaddr.sin_port = htons((uint16_t) port);   //8888
    skaddr.sin_addr.s_addr = inet_addr(SERV);   //0.0.0.0

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
        memset(buff, 0, sizeof(buff));
        int len = (int) recv(sock_client, buff, sizeof(buff), 0);
        fputs(buff, stdout);
        //send(sock_client,buff,len,0);
        //TODO: 实现HTTP request parser来响应不同的请求，这里只写入了固定的响应
        http_send(sock_client, "<h1>Hello World!</h1>\n<p>have fun</p>");//发送HTTP response
        close(sock_client);//response发送完毕，关闭客户端连接
    }
}

void http_send(int sock_client, char *content)
{
    char HTTP_HEADER[BUFF_SIZE], HTTP_INFO[BUFF_SIZE];
    size_t len = strlen(content);
    sprintf(HTTP_HEADER, http_res_tmpl, len, "text/html");
    len = (size_t) sprintf(HTTP_INFO, "%s%s", HTTP_HEADER, content);
    send(sock_client, HTTP_INFO, len, 0);
}

void handle_signal(int sign)
{
    fprintf(stdout, "\nRecieved signal: %d\nBye~\n", sign);
    close(sockfd);
    exit(0);
}