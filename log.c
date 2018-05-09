//
// Created by ruofeng on 18-4-22.
//
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "log.h"
#include "request.h"

char logstr[1024];

void log_string(char *logstr)
{
    write_log(logstr);
}

void log_request(struct http_req_hdr *header)
{
    char method_str[10];
    switch (header->method) {
    case HTTP_METHOD_GET:strcpy(method_str, "GET");
        break;
    case HTTP_METHOD_POST:strcpy(method_str, "POST");
        break;
    case HTTP_METHOD_HEAD:strcpy(method_str, "HEAD");
        break;
    default:strcpy(method_str, "UNKOWN");
        break;
    }
    sprintf(logstr, "%s:%s", method_str, header->uri);
    write_log(logstr);
}

char *get_log_time()
{
    time_t cur_time = time(NULL);
    char *time_str;
    time_str = (char *) malloc(20 * sizeof(char));
    strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", localtime(&cur_time));
    return time_str;
}

void write_log(char *logstr)
{
    FILE *log_file = fopen("../server.log", "a");
    if (log_file == NULL) {
        fprintf(stderr, "open log file filed");
        return;
    }
    fprintf(log_file, "%s %s\n", get_log_time(), logstr);
    fclose(log_file);
}