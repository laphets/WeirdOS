/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/6
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _SOCKET_H
#define _SOCKET_H

#include "lib.h"
#include "tcp.h"


#define SOCKET_MAX_NUM 10
#define SOCKET_MAGIC_RAND_SEED 0xDEADBEEF
#define SOCKET_MAX_PORT 65535

#define SOCKET_TCP_STATUS_INIT 0
#define SOCKET_TCP_STATUS_SENT_SYN 1
#define SOCKET_TCP_STATUS_RECV_SYNACK 2
#define SOCKET_TCP_STATUS_SENT_REQUEST 3
#define SOCKET_TCP_STATUS_SENT_FIN 4


typedef struct socket {
    uint8_t present;
    int32_t sid;
    uint16_t port;
    uint32_t recv_buffer_size;
    void* recv_buffer;
    uint32_t recv_buf_end_ptr;

    /* The following for TCP status ctrl */
    volatile uint32_t tcp_status;
    uint32_t content_length;
    uint32_t header_length;
    uint32_t target_length;
    uint32_t data_packet_num;
    uint32_t my_seq_num, others_seq_num;
    uint32_t first_data_packet_seq_num;
    uint8_t target_ip[IPv4_ADDR_SIZE];
    uint16_t target_port;
    uint32_t tcp_common_length;
    volatile uint8_t has_sent_fin;

    /* The following for DNS status ctrl */
    volatile uint32_t dns_status;
    uint8_t dns_res_ip[IPv4_ADDR_SIZE];


} socket_t;

socket_t socket_list[SOCKET_MAX_NUM];

uint16_t rand_port();
void init_socket();
socket_t* get_socket(uint32_t sid);
int32_t socket_open();
void socket_close(int32_t sid);
socket_t* get_socket_by_port(uint16_t port);
void* socket_http_send(int32_t sid, uint8_t* target_ip, uint16_t target_port, uint8_t* data, uint32_t length, uint32_t* res_length);
uint32_t socket_dns_send(int32_t sid, uint8_t* domain, uint8_t* res_ip);

#endif //MP3_SOCKET_H
