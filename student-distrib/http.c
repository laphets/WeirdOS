/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/7
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "http.h"

http_res_t http_request(uint8_t* url) {
    char url_string[100];
    strcpy((int8_t*)url_string, (const int8_t*)url);
    struct yuarel parsed_url;

    if (-1 == yuarel_parse(&parsed_url, url_string)) {
        http_res_t res;
        res.present = 0;
        return res;
    }
    if(parsed_url.port == 0)
        parsed_url.port = 80;

    ip_wrapper_t dns_record = dns_query((uint8_t*)parsed_url.host);
    uint8_t* target_ip = dns_record.ip_addr;
    char* http_request = (char*)kmalloc(strlen((int8_t*)url) + 100);
    if(parsed_url.path == NULL || strlen(parsed_url.path) == 0) {
        sprintf((uint8_t*)http_request, "GET / HTTP/1.1\r\nHost: %s\r\n\r\n", parsed_url.host);
    } else {
        sprintf((uint8_t*)http_request, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", parsed_url.path, parsed_url.host);
    }

//    uint8_t target_ip[4] = {18, 220, 149, 166};
//    char http_request[] = "GET / HTTP/1.1\r\nHost: lumetta.web.engr.illinois.edu\r\n\r\n";
//    kprintf("http_request: %s\n", http_request);

    uint32_t res_length = 0;
    int32_t socket = socket_open();
    uint8_t* res_data = socket_http_send(socket, target_ip, parsed_url.port, (uint8_t*)http_request, strlen(http_request), &res_length);

    kprintf("content_length: %d, data_length: %d, header_length: %d\n", get_socket(socket)->content_length, res_length, get_socket(socket)->header_length);

//    kprintf("%s\n", res_data);
//    kprintf("RES DATA, size: %d!!!\n", res_length);
//    kprintf("%s\n", res_data + res_length - 50);
    socket_close(socket);
    kfree(http_request);

    http_res_t res;
    res.present = 1;
    res.data = res_data;
    res.length = res_length;

    return res;
}
