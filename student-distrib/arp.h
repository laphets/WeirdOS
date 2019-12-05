/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/3
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _ARP_H
#define _ARP_H

#include "lib.h"
#include "ethernet.h"
#include "ip.h"
#include "network.h"

#define ARP_OP_REQUEST 1
#define ARP_OP_REPLY 2

#define ARP_PROTOCOL_TYPE_IPv4 0x0800
#define ARP_HARDWARE_TYPE_ETHERNET 1

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

void arp_send(uint8_t* mac_dest, uint8_t* ip_dest, uint16_t operation);

void arp_recv(arp_t* arp_packet);

void arp_reply(uint8_t* target_mac, uint8_t* target_ip);
void arp_request(uint8_t* target_ip);

#define ARP_TABLE_SIZE 40

typedef struct arp_table_entry {
    uint8_t protocol_addr[4];
    uint8_t hardware_addr[6];
} arp_table_entry_t;

arp_table_entry_t arp_table[ARP_TABLE_SIZE];

int32_t arp_table_size;

mac_wrapper_t arp_find(uint8_t* target_ip);

void init_arp();

void print_arp_table();

#endif //MP3_ARP_H
