//
// Created by ruofeng on 18-4-22.
//

#ifndef C_SERVER_LOG_H
#define C_SERVER_LOG_H

#include "request.h"

void log_string(char *);
void log_request(struct http_req_hdr *);
char *get_log_time();
void write_log(char *logstr);

#endif //C_SERVER_LOG_H
