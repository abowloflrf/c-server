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

#define BODY_BUFF_SIZE 8192
#define HEADER_BUFF_SIZE 512


struct http_rsp_hdr{
    uint16_t status;
};

size_t response_handler(char*,struct http_req_hdr*);

#endif //C_SERVER_RESPONSE_H
