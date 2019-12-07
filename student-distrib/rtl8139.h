/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/1
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _RTL8139_H
#define _RTL8139_H

#include "lib.h"
#include "pci.h"
#include "kheap.h"
#include "paging.h"
#include "ethernet.h"

#define RTL8139_BUS_ID 0
#define RTL8139_DEVICE_ID 3
#define RTL8139_RXBUFFER_SIZE_MALLOC 8192+16+1500 /* Enable WRAP */
#define RTL8139_RXBUFFER_SIZE 8192
#define TOK 4
#define ROK 1
#define RX_READ_POINTER_MASK (~3)
#define CAPR 0x38

uint8_t self_mac_addr[6];
uint8_t broadcast_mac_addr[6];

void rtl8139_handler();

void rtl8139_send(void* packet, uint32_t length);
void rtl8139_recv();

void init_rtl8139();

#endif //MP3_RTL8139_H
