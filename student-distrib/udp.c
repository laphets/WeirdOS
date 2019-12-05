/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/4
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "udp.h"

void udp_send(uint16_t src_port, uint16_t target_port, uint8_t* target_ip, uint8_t* data, uint32_t length) {
    uint32_t total_length = sizeof(udp_t) + length;
    udp_t* udp_packet = kmalloc(total_length);
    memset(udp_packet, 0, total_length);

    udp_packet->src_port = htons(src_port);
    udp_packet->dst_port = htons(target_port);
    udp_packet->length = htons(total_length);
    udp_packet->checksum = 0;

    memcpy(udp_packet->data, data, length);

    ip_send(target_ip, IP_PROTOCOL_UDP, udp_packet, total_length);
}

void udp_recv(udp_t* udp_packet) {
    /* Restore the endian issue */
    udp_packet->src_port = ntohs(udp_packet->src_port);
    udp_packet->dst_port = ntohs(udp_packet->dst_port);
    udp_packet->length = ntohs(udp_packet->length);
    udp_packet->checksum = ntohs(udp_packet->checksum);
    kprintf("UDP Packet Recv!!!, sport: %d, dport: %d\n", udp_packet->src_port, udp_packet->dst_port);
}
