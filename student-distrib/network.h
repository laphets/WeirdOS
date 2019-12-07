/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/4
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _NETWORK_H
#define _NETWORK_H

#include "lib.h"

#define MAC_ADDR_SIZE 6
#define IPv4_ADDR_SIZE 4

typedef struct mac_wrapper {
    uint8_t valid;
    uint8_t mac_addr[MAC_ADDR_SIZE];
} mac_wrapper_t;

typedef struct ip_wrapper {
    uint8_t valid;
    uint8_t ip_addr[IPv4_ADDR_SIZE];
} ip_wrapper_t;

#endif //MP3_NETWORK_H
