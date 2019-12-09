/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/7
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "http.h"

/**
 * Send HTTP1.1 request to some vertain place
 * @param domain
 * @return
 */
http_res_t http_request(uint8_t* domain) {
    ip_wrapper_t dns_record = dns_query(domain);
    uint8_t* target_ip = dns_record.ip_addr;
    char* http_request = (char*)kmalloc(strlen((int8_t*)domain) + 100);
    sprintf((uint8_t*)http_request, "GET / HTTP/1.1\r\nHost: %s\r\n\r\n", domain);
//    uint8_t target_ip[4] = {18, 220, 149, 166};
//    char http_request[] = "GET / HTTP/1.1\r\nHost: lumetta.web.engr.illinois.edu\r\n\r\n";
//    kprintf("http_request: %s\n", http_request);

    uint32_t res_length = 0;
    int32_t socket = socket_open();
    uint8_t* res_data = socket_http_send(socket, target_ip, 80, (uint8_t*)http_request, strlen(http_request), &res_length);

//    kprintf("%s\n", res_data);
//    kprintf("RES DATA, size: %d!!!\n", res_length);
//    kprintf("%s\n", res_data + res_length - 50);
    socket_close(socket);
    kfree(http_request);

    http_res_t res;
    res.data = res_data;
    res.length = res_length;

    return res;
}
