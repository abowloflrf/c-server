//
// Created by ruofeng on 18-4-22.
//

#ifndef C_SERVER_RESPONSE_H
#define C_SERVER_RESPONSE_H

#include "request.h"
#include <stdint.h>

#define HTTP_STATUS_OK              200
#define HTTP_STATUS_NOT_MODIFIED    304
#define HTTP_STATUS_NOT_FOUND       404

#define HEADER_BUFF_SIZE 512

struct http_rsp_hdr
{
    uint16_t status;
    char *date;
    char *last_modified;
    char *content_type;
    ssize_t content_length;
};

void response_handler(int, struct http_req_hdr *);
void get_filetype(char *filename, char *filetype);
void send_error_response(int socket_fd, char *status, char *msg);
ssize_t write_to_socket(int fd, void *buf, size_t n);
#endif //C_SERVER_RESPONSE_H
