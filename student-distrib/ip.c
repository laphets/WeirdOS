/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/4
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "ip.h"

/**
 * Init self ip addr
 */
void init_ip() {
    self_ip_addr[0] = 10;
    self_ip_addr[1] = 0;
    self_ip_addr[2] = 2;
    self_ip_addr[3] = 15;
}

/**
 * Print a given ip
 * @param ip
 */
void print_ip(uint8_t* ip) {
    kprintf("ip_addr: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
}

/**
 * Compare two ip addr
 * @param ip1
 * @param ip2
 * @return if equal, return 0
 */
uint8_t ip_cmp(uint8_t* ip1, uint8_t* ip2) {
    int i = 0;
    for(i = 0; i < IPv4_ADDR_SIZE; i++) {
        if(ip1[i] != ip2[i]) {
            return 1;
        }
    }
    return 0;
}

/**
 * IP layer recv handler
 * @param ip_packet
 */
void ip_recv(ip_t* ip_packet) {

    /* Fix some endian issue */
    *((uint8_t*)(&ip_packet->version_IHT_ptr)) = ntohb(*((uint8_t*)(&ip_packet->version_IHT_ptr)), 4);
    *((uint8_t*)(&ip_packet->flags_fghigh5_ptr)) = ntohb(*((uint8_t*)(&ip_packet->flags_fghigh5_ptr)), 3);

    ip_packet->total_length = ntohs(ip_packet->total_length);

    uint32_t data_ptr = (uint32_t)ip_packet + 4 * (uint32_t)ip_packet->IHL;
    kprintf("IP packet recv!!!! version: %d, IHL: %d\n", ip_packet->version, ip_packet->IHL);
    switch(ip_packet->protocol) {
        case IP_PROTOCOL_UDP: {
            /* UDP Packet */
            udp_recv((udp_t*)data_ptr);
            break;
        }
        case IP_PROTOCOL_TCP: {
            /* TCP Packet */
            tcp_recv((tcp_t*)data_ptr, ip_packet->total_length - ip_packet->IHL * 4);
            break;
        }
        case IP_PROTOCOL_ICMP: {
            /* ICMP Packet */

            break;
        }
        case IP_PROTOCOL_IPv6: {
            /* IPv6 Packet */

            break;
        }
        default: {
            /* Unsupport Packet */
            break;
        }
    }

}

/*
 * Ref: http://www.microhowto.info/howto/calculate_an_internet_protocol_checksum_in_c.html
 */
void ip_fresh_checksum(ip_t* ip_packet) {
    /**
     * The checksum field is the 16 bit one's complement of the one's complement sum of all 16 bit words in the header.
     * For purposes of computing the checksum, the value of the checksum field is zero.
     * See RFC 791.
     */
    ip_packet->header_checksum = 0;

    uint32_t length = sizeof(ip_t);  /* We can't use ip_packet->IHL * 4, since it's big endian, so we use this polyfill */
    uint32_t i = 0;

    // Cast the data pointer to one that can be indexed.
    char* data=(char*)ip_packet;

    // Initialise the accumulator.
    uint32_t acc=0xffff;

    // Handle complete 16-bit blocks.
    for (i=0;i+1<length;i+=2) {
        uint16_t word;
        memcpy(&word,data+i,2);
        acc+=ntohs(word);
        if (acc>0xffff) {
            acc-=0xffff;
        }
    }

    // Handle any partial block at the end of the data.
    if (length&1) {
        uint16_t word=0;
        memcpy(&word,data+length-1,1);
        acc+=ntohs(word);
        if (acc>0xffff) {
            acc-=0xffff;
        }
    }

    // Return the checksum in network byte order.
    ip_packet->header_checksum = htons(~acc);
}

/**
 * IP layer send method
 * @param target_ip
 * @param protocol upper layer protocol
 * @param data
 * @param length
 */
void ip_send(uint8_t* target_ip, uint16_t protocol, uint8_t* data, uint32_t length) {
    uint32_t total_size = sizeof(ip_t) + length;
    ip_t* ip_packet = kmalloc(total_size);
    memset(ip_packet, 0, total_size);

    ip_packet->version = 4; /* Indicate IPv4 */
    ip_packet->IHL = 5; /* No additional optional field */
    ip_packet->DSCP = 0;
    ip_packet->ECN = 0;
    ip_packet->total_length = htons(total_size);
    ip_packet->identification = 0;
    ip_packet->flags = (1 << 1); /* Don't Fragment */
    ip_packet->fragment_offset_high5 = 0;
    ip_packet->fragment_offset_low8 = 0;
    ip_packet->ttl = 64; /* default ttl */
    ip_packet->protocol = protocol;
    memcpy(ip_packet->ip_src, self_ip_addr, IPv4_ADDR_SIZE);
    memcpy(ip_packet->ip_dst, target_ip, IPv4_ADDR_SIZE);

    /* Then copy the data */
    uint32_t target_data_ptr = (uint32_t)ip_packet + (uint32_t)ip_packet->IHL * 4;
    memcpy((void*)target_data_ptr, (const void*)data, length);

    /* Fix some bad endian */
    *((uint8_t*)(&ip_packet->version_IHT_ptr)) = htonb(*((uint8_t*)(&ip_packet->version_IHT_ptr)), 4);
    *((uint8_t*)(&ip_packet->flags_fghigh5_ptr)) = htonb(*((uint8_t*)(&ip_packet->flags_fghigh5_ptr)), 3);

    /* kprintf("trp compare: ip_packet->ip_dst: 0x%x, target_data_ptr: 0x%x\n", ip_packet->ip_dst, target_data_ptr); */

    kprintf("Target ip:");
    print_ip(ip_packet->ip_dst);

    /* Then solve the header checksum */
    ip_fresh_checksum(ip_packet);

    uint8_t real_ip[4];
    memcpy(real_ip, target_ip, IPv4_ADDR_SIZE);

    if(target_ip[0] != 10) {
        /* Plain checking for subnet */
        /* Then we send to the gateway */
        real_ip[0] = 10;
        real_ip[1] = 0;
        real_ip[2] = 2;
        real_ip[3] = 2;
    }

    /* In the same subnet */
    /* Then fetch MAC addr for target */
    mac_wrapper_t arp_res;
    int32_t arp_cnt = 0, total_trying = 0;
    while(1) {
        if(total_trying++ > 100) {
            /* Try to much, just say fail */
            kprintf("ARP no response, fail ip sending\n");
            return;
        }
        /* kprintf("Fetching arp: %d\n", arp_cnt); */
        arp_res = arp_find(real_ip);
        if(arp_res.valid == 1) {
            break;
        }
        if(arp_cnt > 4) {
            /* Wait for the response */
            continue;
        }
        arp_cnt++;
        arp_request(real_ip);
    }

    ethernet_send(arp_res.mac_addr, ETHERTYPE_IPv4, (uint8_t*)ip_packet, total_size);



    kfree(ip_packet);
}

