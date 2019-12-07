/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/3
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "ethernet.h"

void print_mac(uint8_t* mac_addr) {
    kprintf("mac_addr: %x:%x:%x:%x:%x:%x\n", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
}

void ethernet_recv(ethernet_t* frame, uint32_t length) {
    kprintf("ethernet frame: ");
    print_mac(frame->mac_dest);

    uint16_t type = ntohs(frame->type);
    switch (type) {
        case ETHERTYPE_ARP: {
            arp_recv((arp_t*)frame->data);
            break;
        }
        case ETHERTYPE_IPv4: {
            /* IPv4 Packet */
            ip_recv((ip_t*)frame->data);
            break;
        }
        default: {
            /* Do nothing and drop that */
        }
    }
}

void ethernet_send(uint8_t* mac_dest, uint16_t type, uint8_t* data, uint32_t length) {
    ethernet_t* frame = kmalloc(sizeof(ethernet_t) + length);
    memcpy(frame->mac_src, self_mac_addr, MAC_ADDR_SIZE);
    memcpy(frame->mac_dest, mac_dest, MAC_ADDR_SIZE);
    frame->type = htons(type);
    memcpy(frame->data, data, length);
    rtl8139_send(frame, sizeof(ethernet_t) + length);
    kfree(frame);
}
