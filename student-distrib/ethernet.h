/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/3
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _ETHERNET_H
#define _ETHERNET_H

#include "lib.h"
#include "rtl8139.h"
#include "arp.h"
#include "network.h"

#define ETHERTYPE_ARP 0x0806
#define ETHERTYPE_IPv4 0x0800

typedef struct ethernet {
    uint8_t mac_dest[6];
    uint8_t mac_src[6];
    uint16_t type;
    uint8_t data[];
} __attribute__((packed)) ethernet_t;

void print_mac(uint8_t* mac_addr);

void ethernet_recv(ethernet_t* frame, uint32_t length);
void ethernet_send(uint8_t* mac_dest, uint16_t type, uint8_t* data, uint32_t length);

#endif //MP3_ETHERNET_H
