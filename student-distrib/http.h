/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/7
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _HTTP_H
#define _HTTP_H

#include "lib.h"
#include "dns.h"
#include "socket.h"

typedef struct http_res {
    uint32_t length;
    void* data;
    void* header;
} http_res_t;

http_res_t http_request(uint8_t* domain);

#endif //MP3_HTTP_H
