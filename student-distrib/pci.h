/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/3
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _PCI_H
#define _PCI_H

#include "lib.h"


uint32_t pci_read(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);

void pci_write(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint32_t data);
void init_pci();

#endif //MP3_PCI_H
