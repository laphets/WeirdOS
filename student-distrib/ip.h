/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/4
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _IP_H
#define _IP_H

#include "lib.h"
#include "ethernet.h"
#include "network.h"
#include "arp.h"
#include "udp.h"
#include "tcp.h"

uint8_t self_ip_addr[IPv4_ADDR_SIZE];

void init_ip();

void print_ip(uint8_t* ip);

uint8_t ip_cmp(uint8_t* ip1, uint8_t* ip2);

typedef struct ip {
    void* version_IHT_ptr[0];
    uint8_t version : 4;    /* For IPv4, this should be 4 */
    uint8_t IHL : 4; /* Internet Header Length, by default it's 5 (5 uint32) which indicates no additional options */
    void* DSCP_ECN_ptr[0];
    uint8_t DSCP : 6; /* Differentiated Services Code Point */
    uint8_t ECN : 2; /* Explicit Congestion Notification, see RFC 3168 */
    uint16_t total_length;  /* Length including header and data, we'd better to restrict it to 576, also including fragmentation */
    uint16_t identification;    /* Not care */
    void* flags_fghigh5_ptr[0];
    uint8_t flags : 3;  /* bit 1: Don't Fragment (DF), bit 2: More Fragments (MF), we should set bit 1 for not fragment */
    uint8_t fragment_offset_high5 : 5;
    uint8_t fragment_offset_low8;  /* In units of eight-byte blocks */
    uint8_t ttl;    /* Time To Live */
    uint8_t protocol;   /* Protocol for downward layer, see RFC 790 */
    uint16_t header_checksum;   /* Checksum for header */
    uint8_t ip_src[IPv4_ADDR_SIZE]; /* Source IP */
    uint8_t ip_dst[IPv4_ADDR_SIZE]; /* Dest IP */

    /* The data contains some option and real data */
    uint8_t data[];
} __attribute__((packed)) ip_t;

#define IP_PROTOCOL_ICMP 0x01
#define IP_PROTOCOL_TCP 0x06
#define IP_PROTOCOL_UDP 0x11
#define IP_PROTOCOL_IPv6 0x29

void ip_recv(ip_t* ip_packet);
void ip_send(uint8_t* target_ip, uint16_t protocol, uint8_t* data, uint32_t length);

#endif //MP3_IP_H
