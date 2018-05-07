//
// Created by ruofeng on 18-5-6.
//

#ifndef C_SERVER_FCGI_H
#define C_SERVER_FCGI_H

/*
 * Number of bytes in a FCGI_Header.  Future versions of the protocol
 * will not reduce this number.
 */
#include <stdio.h>
#include "io.h"
#include "request.h"

#define FCGI_MAX_LENGTH 0xFFFF

#define FCGI_HEADER_LEN  8

/*
 * Value for version component of FCGI_Header
 */
#define FCGI_VERSION_1           1
/*
 * Values for type component of FCGI_Header
 */
#define FCGI_BEGIN_REQUEST       1
#define FCGI_ABORT_REQUEST       2
#define FCGI_END_REQUEST         3
#define FCGI_PARAMS              4
#define FCGI_STDIN               5
#define FCGI_STDOUT              6
#define FCGI_STDERR              7
#define FCGI_DATA                8
#define FCGI_GET_VALUES          9
#define FCGI_GET_VALUES_RESULT  10
#define FCGI_UNKNOWN_TYPE       11
#define FCGI_MAXTYPE (FCGI_UNKNOWN_TYPE)

/*
 * Value for requestId component of FCGI_Header
 */
#define FCGI_NULL_REQUEST_ID     0

typedef struct
{
    unsigned char version;
    unsigned char type;
    unsigned char requestIdB1;
    unsigned char requestIdB0;
    unsigned char contentLengthB1;
    unsigned char contentLengthB0;
    unsigned char paddingLength;
    unsigned char reserved;
} FCGI_Header;

typedef struct
{
    unsigned char roleB1;
    unsigned char roleB0;
    unsigned char flags;
    unsigned char reserved[5];
} FCGI_BeginRequestBody;

typedef struct
{
    FCGI_Header header;
    FCGI_BeginRequestBody body;
} FCGI_BeginRequestRecord;
/*
 * Mask for flags component of FCGI_BeginRequestBody
 */
#define FCGI_KEEP_CONN  1

/*
 * Values for role component of FCGI_BeginRequestBody
 */
#define FCGI_RESPONDER  1
#define FCGI_AUTHORIZER 2
#define FCGI_FILTER     3

typedef struct
{
    unsigned char appStatusB3;
    unsigned char appStatusB2;
    unsigned char appStatusB1;
    unsigned char appStatusB0;
    unsigned char protocolStatus;
    unsigned char reserved[3];
} FCGI_EndRequestBody;

typedef struct
{
    FCGI_Header header;
    FCGI_EndRequestBody body;
} FCGI_EndRequestRecord;

/*
 * Values for protocolStatus component of FCGI_EndRequestBody
 */
#define FCGI_REQUEST_COMPLETE 0
#define FCGI_CANT_MPX_CONN    1
#define FCGI_OVERLOADED       2
#define FCGI_UNKNOWN_ROLE     3

/*
 * Variable names for FCGI_GET_VALUES / FCGI_GET_VALUES_RESULT records
 */
#define FCGI_MAX_CONNS  "FCGI_MAX_CONNS"
#define FCGI_MAX_REQS   "FCGI_MAX_REQS"
#define FCGI_MPXS_CONNS "FCGI_MPXS_CONNS"

typedef struct
{
    unsigned char type;
    unsigned char reserved[7];
} FCGI_UnknownTypeBody;

typedef struct
{
    FCGI_Header header;
    FCGI_UnknownTypeBody body;
} FCGI_UnknownTypeRecord;


#define PHP_FPM_HOST "127.0.0.1"
#define PHP_FPM_PORT 9000

/// 打开php-fpm sock连接
int open_fpm_sock();

/// 构造FastCGI请求头部
FCGI_Header makeHeader(int type, int requestId, size_t contentLength, size_t paddingLength);

/// 构造FastCGI开始请求体
FCGI_BeginRequestBody makeBeginRequestBody(int role, int keepConn);

/// 发送开始请求记录
/// \param wr
/// \param fd
/// \param requestId
/// \return
int sendBeginRequestRecord(int fd, int requestId);

/// 发送请求结束记录
/// \param wr
/// \param fd
/// \param requestId
/// \return
int sendEndRequestRecord(int fd, int requestId);

///发送名值对参数
int makeNameValueBody(char *name, size_t nameLen, char *value, size_t valueLen,
                      unsigned char *bodyBuffPtr, size_t *bodyLenPtr);
int sendParamsRecord(int fd, char *name, size_t nlen, char *value, size_t vlen);
int sendParams(int fd,char *name,char *value);

/// 发送空的params记录
int sendEmptyParamsRecord(int fd);

int sendStdinRecord(int fd, char *data, int len);
int sendEmptyStdinRecord(int fd);

/// 向php-fpm发送FastCGI请求
/// \param rp rio缓冲
/// \param hdr http请求头部
/// \param sock fpm socket
/// \return
int send_fastcgi(rio_t *rp, struct http_req_hdr *hdr, int sock);

typedef ssize_t (*send_to_client)(int, size_t, char *, size_t, char *, FCGI_EndRequestBody *);

/// 从php-fpm接受FastCGI响应
/// \param cfd 客户端的socket
/// \param fd  php-fpm的socket
/// \return
int recv_fastcgi(send_to_client stc, int cfd, int fd);


//sendStdinRecord
//sendEmptyStdinRecord


#endif //C_SERVER_FCGI_H
