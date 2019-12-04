/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/3
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "arp.h"

void arp_send(uint8_t* mac_dest, uint8_t* ip_dest) {
    arp_t* arp = kmalloc(sizeof(arp_t));
    arp->hardware_type = htons(1); /* Ethernet is 1 */
    arp->protocol_type = htons(0x0800); /* For IPv4 */
    arp->hardware_addr_length = 6; /* Ethernet MAC has length 6 */
    arp->protocol_addr_length = 4; /* IPv4 has length 4 */
    arp->operation = htons(1); /* 1 for request, 2 for reply */
    memcpy(arp->sender_hardware_addr, self_mac_addr, MAC_ADDR_LENGTH);
//    memcpy(arp->sender_protocol_addr)
    memcpy(arp->target_hardware_addr, mac_dest, MAC_ADDR_LENGTH);
    memcpy(arp->target_protocol_addr, ip_dest, IPv4_ADDR_LENGTH);
    ethernet_send(mac_dest, 0x0806, arp, sizeof(arp_t));

    kfree(arp);
}
