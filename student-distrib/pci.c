/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/3
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "pci.h"

uint32_t pci_read(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset) {
    /**
     *
     *   31	        30 - 24	    23 - 16	    15 - 11	        10 - 8	        7 - 0
     *   Enable Bit	Reserved	Bus Number	Device Number	Function Number	Register Offset¹
     *   We should set for the enable Bit, and insert the Bus, Device, Function and Offset.
     *   Register Offset has to point to consecutive DWORDs, ie. bits 1:0 are always 0b00
     */

    uint32_t addr = 0x80000000 | ((uint32_t)bus << 16) | ((uint32_t)(device & 0x0F) << 11) | ((uint32_t)(func & 0x07) << 8) | (offset & 0xFC);
    outl(addr, 0xCF8);
    return inl(0xCFC);
}

void pci_write(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint32_t data) {
    uint32_t addr = 0x80000000 | ((uint32_t)bus << 16) | ((uint32_t)(device & 0x0F) << 11) | ((uint32_t)(func & 0x07) << 8) | (offset & 0xFC);
    outl(addr, 0xCF8);
    outl(data, 0xCFC);
}

void init_pci() {
    kprintf("Begin scan pci devices...\n");
    uint32_t bus = 0, device = 0;
    for(bus = 0; bus < 256; bus++)
        for(device = 0; device < 16; device++) {
            uint32_t res = pci_read(bus, device, 0, 0);
            if(res != 0xFFFFFFFF)
                kprintf("[pci] bus: %d, device: %d, res: 0x%x\n", bus, device, res);
        }
}
