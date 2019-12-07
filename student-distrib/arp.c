/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/3
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "arp.h"

void init_arp() {
    arp_table_size = 0;
}

void print_arp_table() {
    kprintf("-----BEGIN ARP TABLE-----\n");
    int32_t i = 0;
    for(i = 0; i < arp_table_size; i++) {
        arp_table_entry_t* arp_entry = &arp_table[i];
        print_mac(arp_entry->hardware_addr);
        print_ip(arp_entry->protocol_addr);
    }

    kprintf("-----END ARP TABLE-----\n");
}

mac_wrapper_t arp_find(uint8_t* target_ip) {
    int32_t i = 0;
    mac_wrapper_t result;
    result.valid = 0;
    for(i = 0; i < arp_table_size; i++) {
        arp_table_entry_t* arp_entry = &arp_table[i];
        if(ip_cmp(arp_entry->protocol_addr, target_ip) == 0) {
            memcpy(result.mac_addr, arp_entry->hardware_addr, MAC_ADDR_SIZE);
            result.valid = 1;
            return result;
        }
    }

    return result;
}

void arp_insert(uint8_t* target_ip, uint8_t* target_mac) {
    if(arp_find(target_ip).valid == 0) {
        /* Not in the table, then we insert a new */
        arp_table_entry_t* arp_entry = &arp_table[arp_table_size++];
        memcpy(arp_entry->protocol_addr, target_ip, IPv4_ADDR_SIZE);
        memcpy(arp_entry->hardware_addr, target_mac, MAC_ADDR_SIZE);
    } else {
        /* In the table, then we update that */
        int32_t i = 0;
        for(i = 0; i < arp_table_size; i++) {
            arp_table_entry_t* arp_entry = &arp_table[i];
            if(ip_cmp(arp_entry->protocol_addr, target_ip) == 0) {
                memcpy(arp_entry->hardware_addr, target_mac, MAC_ADDR_SIZE);
                return;
            }
        }
    }
}

void arp_send(uint8_t* mac_dest, uint8_t* ip_dest, uint16_t operation) {
    arp_t* arp_packet = kmalloc(sizeof(arp_t));
    arp_packet->hardware_type = htons(ARP_HARDWARE_TYPE_ETHERNET); /* Ethernet is 1 */
    arp_packet->protocol_type = htons(ARP_PROTOCOL_TYPE_IPv4); /* For IPv4 */
    arp_packet->hardware_addr_length = 6; /* Ethernet MAC has length 6 */
    arp_packet->protocol_addr_length = 4; /* IPv4 has length 4 */
    arp_packet->operation = htons(ARP_OP_REQUEST); /* 1 for request, 2 for reply */
    memcpy(arp_packet->sender_hardware_addr, self_mac_addr, MAC_ADDR_SIZE);
    memcpy(arp_packet->sender_protocol_addr, self_ip_addr, IPv4_ADDR_SIZE);
    memcpy(arp_packet->target_hardware_addr, mac_dest, MAC_ADDR_SIZE);
    memcpy(arp_packet->target_protocol_addr, ip_dest, IPv4_ADDR_SIZE);
    ethernet_send(mac_dest, 0x0806, (uint8_t*)arp_packet, sizeof(arp_t));

    kfree(arp_packet);
}


void arp_reply(uint8_t* target_mac, uint8_t* target_ip) {
    arp_send(target_mac, target_ip, ARP_OP_REPLY);
}

void arp_request(uint8_t* target_ip) {
    arp_send(broadcast_mac_addr, target_ip, ARP_OP_REQUEST);
}

void arp_recv(arp_t* arp_packet) {
    /* ARP packet recv */
    kprintf("ARP packet recv, operation %d\n", ntohs(arp_packet->operation));

    if(ntohs(arp_packet->operation) == ARP_OP_REQUEST) {
        /* ARP Request to here, we shoud see whether we can reply that */
        if(ntohs(arp_packet->hardware_type) == ARP_HARDWARE_TYPE_ETHERNET  && ntohs(arp_packet->protocol_type) == ARP_PROTOCOL_TYPE_IPv4) {
            if(ip_cmp(arp_packet->target_protocol_addr, self_ip_addr) == 0) {
                /* Ask us for arp request, then we should reply message */
                arp_reply(arp_packet->sender_hardware_addr, arp_packet->sender_protocol_addr);
            }
        }
    } else if (ntohs(arp_packet->operation) == ARP_OP_REPLY) {
        /* Some reply to here, we should record that */
        if (ntohs(arp_packet->hardware_type) == ARP_HARDWARE_TYPE_ETHERNET &&
            ntohs(arp_packet->protocol_type) == ARP_PROTOCOL_TYPE_IPv4) {
            arp_insert(arp_packet->sender_protocol_addr, arp_packet->sender_hardware_addr);
        }
    }
}
