/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/4
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _UDP_H
#define _UDP_H

#include "lib.h"
#include "network.h"
#include "ip.h"

typedef struct udp {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;    /* bytes of the UDP header and UDP data */
    uint16_t checksum;
    uint8_t data[0];
} __attribute__((packed)) udp_t;

void udp_send(uint16_t src_port, uint16_t target_port, uint8_t* target_ip, uint8_t* data, uint32_t length);
void udp_recv(udp_t* udp_packet);

#endif //MP3_UDP_H
