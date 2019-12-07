/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/6
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _DNS_H
#define _DNS_H

#include "lib.h"
#include "udp.h"
#include "socket.h"

#define DNS_PORT 53

uint8_t default_dns_server[IPv4_ADDR_SIZE];

typedef struct dns {
    uint16_t id;
    uint16_t control;
    uint16_t question_count;
    uint16_t answer_count;
    uint16_t authority_count;
    uint16_t additional_count;

    uint8_t payload[0];
} __attribute__((packed)) dns_t;

typedef struct dns_ans {
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t data_length;
    uint8_t addr[0];
} __attribute__((packed)) dns_ans_t;

void init_dns();
void dns_send(uint16_t src_port, uint8_t* domain);
void dns_recv(dns_t* dns_packet, uint32_t length, uint8_t* res_ip);
ip_wrapper_t dns_query(uint8_t* domain);


#endif //MP3_DNS_H
