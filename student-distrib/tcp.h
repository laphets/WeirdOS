/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/4
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _TCP_H
#define _TCP_H

#include "lib.h"
#include "network.h"
#include "ip.h"

#define TCP_FLAG_FIN (1 << 0)
#define TCP_FLAG_SYN (1 << 1)
#define TCP_FLAG_RST (1 << 2)
#define TCP_FLAG_PSH (1 << 3)
#define TCP_FLAG_ACK (1 << 4)
#define TCP_FLAG_URG (1 << 5)
#define TCP_FLAG_ECE (1 << 6)
#define TCP_FLAG_CWR (1 << 7)

typedef struct tcp {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq_num;
    uint32_t ack_num;
    void* data_reserved_ptr[0];
    uint8_t data_offset : 4;    /* This field gets its name from the fact that it is also the offset from the start of the TCP segment to the actual data. */
    uint8_t reserved_ns : 4;    /* Should be 0 */
    uint8_t flags;
    uint16_t window_size;   /* For flow control */
    uint16_t checksum;
    uint16_t urgent_pointer; /* if URG set */

    uint8_t options[0];

    uint8_t data[0];    /* The data may conatins some optional field, we should always access data by data_offset */
} __attribute__((packed)) tcp_t;

typedef struct tcp_checksum_pre_header {
    uint8_t src_ip[4];
    uint8_t dst_ip[4];
    uint8_t zeros;
    uint8_t protocol; /* Should be 6 */
    uint16_t length; /* the length of the TCP header and data (measured in octets) */

} __attribute__((packed)) tcp_checksum_pre_header_t;

void tcp_send(uint16_t src_port, uint16_t target_port, uint8_t* target_ip, uint32_t seq_num, uint32_t ack_num, uint8_t flags, uint8_t* data, uint32_t length);
void tcp_recv(tcp_t* tcp_packet);

void tcp_test();

#endif //MP3_TCP_H
