//
// Created by ruofeng on 18-5-6.
//

#ifndef C_SERVER_IO_H
#define C_SERVER_IO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#define RIO_BUFSIZE 8192

typedef struct
{
    int rio_fd;                 // 内部缓冲区对应的描述符
    size_t rio_cnt;                // 可以读取的字节数
    char *rio_bufptr;           // 下一个可以读取的字节地址
    char rio_buf[RIO_BUFSIZE];  // 内部缓冲区
} rio_t;

//不带缓冲的read与write
ssize_t rio_readn(int fd, void *usrbuf, size_t n);
ssize_t rio_writen(int fd, void *usrbuf, size_t n);

//初始化rio缓冲结构体
void rio_readinitb(rio_t *rp, int fd);

//带缓冲的读文件
static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n);
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n);

#endif //C_SERVER_IO_H
