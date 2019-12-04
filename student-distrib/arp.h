/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/3
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _ARP_H
#define _ARP_H

#include "lib.h"
#include "ethernet.h"

typedef struct arp {
    uint16_t hardware_type;
    uint16_t protocol_type;
    uint8_t hardware_addr_length;
    uint8_t protocol_addr_length;
    uint16_t operation;
    uint8_t sender_hardware_addr[6];
    uint8_t sender_protocol_addr[4];
    uint8_t target_hardware_addr[6];
    uint8_t target_protocol_addr[4];
} __attribute__((packed)) arp_t;

void arp_send(uint8_t* mac_dest, uint8_t* ip_dest);

#endif //MP3_ARP_H
