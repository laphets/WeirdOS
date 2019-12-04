/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/1
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 * Ref: https://www.cs.usfca.edu/~cruse/cs326f04/RTL8139_ProgrammersGuide.pdf
 * Ref: https://wiki.osdev.org/RTL8139
 */
#include "rtl8139.h"

uint32_t io_addr;
uint32_t rx_buffer;
uint32_t rx_read_ptr_offset = 0;


uint8_t transmit_start_regitsers[4] = {0x20, 0x24, 0x28, 0x2C};
uint8_t transmit_cmd_registers[4] = {0x10, 0x14, 0x18, 0x1C};
uint8_t cur_reg_pos;


void read_mac_addr() {

    /**
     *   FF:FF:FF:FF:FF:FF
     *   Offset (from IO base)	Size	Name
     *   0x00	6	MAC0-5
     */

    uint32_t mac0to3 = inl(io_addr + 0x00);
    uint16_t mac4to5 = inw(io_addr + 0x04);

    self_mac_addr[0] = (uint8_t)(mac0to3 >> 0);
    self_mac_addr[1] = (uint8_t)(mac0to3 >> 8);
    self_mac_addr[2] = (uint8_t)(mac0to3 >> 16);
    self_mac_addr[3] = (uint8_t)(mac0to3 >> 24);
    self_mac_addr[4] = (uint8_t)(mac4to5 >> 0);
    self_mac_addr[5] = (uint8_t)(mac4to5 >> 8);

    int i = 0;
    for(i = 0; i < 6; i++)
        broadcast_mac_addr[i] = 0xFF;

    kprintf("mac_addr: %x:%x:%x:%x:%x:%x\n", self_mac_addr[0], self_mac_addr[1], self_mac_addr[2], self_mac_addr[3], self_mac_addr[4], self_mac_addr[5]);
}


void rtl8139_handler() {
    kprintf("rtl8139 Interrupt comes!!!!\n");

    uint16_t status = inw(io_addr + 0x3E);

    if(status & TOK) {
        /* Packet Send Succ */
        kprintf("packet send!!!\n");
    }

    if(status & ROK) {
        /* Packet Recv Succ */
        kprintf("packet recv!!!\n");
        rtl8139_recv();
    }

    outw(0x5, io_addr + 0x3E);

    send_eoi(0xB);
}

void rtl8139_send(void* packet, uint32_t length) {
    uint32_t phys_addr = vitrual2phys(packet);
    outl(phys_addr, io_addr + transmit_start_regitsers[cur_reg_pos]);
    outl(length, io_addr + transmit_cmd_registers[cur_reg_pos]);

    cur_reg_pos++;
    if(cur_reg_pos > 3)
        cur_reg_pos = 0;
}

void rtl8139_recv() {
    uint32_t rx_read_ptr = rx_buffer + rx_read_ptr_offset;
    uint16_t packet_length = *((uint16_t*)(rx_read_ptr + 2));

    uint32_t income_packet = (rx_read_ptr + 4);
    if(rx_read_ptr_offset + packet_length > RTL8139_RXBUFFER_SIZE) {
        /* wrap around to end of rx_buffer */
        memcpy( rx_buffer + RTL8139_RXBUFFER_SIZE ,rx_buffer,
                (rx_read_ptr_offset + packet_length - RTL8139_RXBUFFER_SIZE) );
    }

    void* real_packet = kmalloc(packet_length - 4);
    memcpy(real_packet, income_packet, packet_length - 4);

    /**
     * Then we pass to link layer
     */

    ethernet_recv(real_packet, packet_length - 4);

    rx_read_ptr_offset = (rx_read_ptr_offset + packet_length + 4 + 3) & RX_READ_POINTER_MASK;
    //4:for header length(PktLength include 4 bytes CRC)
    //3:for dword alignment

    outw( rx_read_ptr_offset - 0x10, io_addr + CAPR); //-4:avoid overflow

    kfree(real_packet);
}

void init_rtl8139() {
    /**
     * We should read offset4 for status + command
     * Status: A register used to record status information for PCI bus related events.
     * Command: Provides control over a device's ability to generate and respond to PCI cycles.
     * In res, we have: bit 31-16 Status; bit 15-0 Command
     */

    uint32_t pci_cmd_conf = pci_read(RTL8139_BUS_ID, RTL8139_DEVICE_ID, 0, 4);
    /**
     *
     *  Here is the layout of the Status register:
     *  Bit 15	Bit 14	Bit 13	Bit 12	Bit 11	Bits 9 and 10	Bit 8	Bit 7	Bit 6	Bit 5	Bit 4	Bit 3	Bits 0 to 2
     *  Detected Parity Error	Signaled System Error	Received Master Abort	Received Target Abort	Signaled Target Abort	DEVSEL Timing	Master Data Parity Error	Fast Back-to-Back Capable	Reserved	66 MHz Capable	Capabilities List	Interrupt Status	Reserved
     *
     *  Here is the layout of the Command register:
     *  Bits 11 to 15	Bit 10	Bit 9	Bit 8	Bit 7	Bit 6	Bit 5	Bit 4	Bit 3	Bit 2	Bit 1	Bit 0
     *  Reserved	Interupt Disable	Fast Back-to-Back Enable	SERR# Enable	Reserved	Parity Error Response	VGA Palette Snoop	Memory Write and Invalidate Enable	Special Cycles	Bus Master	Memory Space	I/O Space
     *
     *  We should enable I/O Space(bit 0), Memory Space(bit 1), Bus Master(bit 2), Memory Write and Invalidate Enable(bit 4)
     */

    pci_cmd_conf |= (1 << 0);
    pci_cmd_conf |= (1 << 1);
    pci_cmd_conf |= (1 << 2);
    pci_cmd_conf |= (1 << 4);
    pci_write(RTL8139_BUS_ID, RTL8139_DEVICE_ID, 0, 4, pci_cmd_conf);

    kprintf("rtl8139 pci setup!\n");

    /**
     * We should then read the io port and memory mapping from pci
     * Memory Space BAR(Bar0) 0x10
     * I/O Space BAR(Bar1) 0x14
     * For rtl8139, it's port based, so we just need to read from Bar0, and the lowest bit = 1 tells us the io based
     * And we can extract the port from that
     */

    uint32_t bar0 = pci_read(RTL8139_BUS_ID, RTL8139_DEVICE_ID, 0, 0x10);
//    uint32_t io_bar = pci_read(RTL8139_BUS_ID, RTL8139_DEVICE_ID, 0, 0x14);
    io_addr = bar0 & (~0x3);

    kprintf("rtl8139 io_addr: 0x%x\n", io_addr);

    /**
     * Then we go inside the real init process
     */

    cur_reg_pos = 0;

    /* Turn on RTL8139 */
    outb(0x0, io_addr + 0x52);

    /* Software Reset! */
    outb(0x10, io_addr + 0x37);
    while((inb(io_addr + 0x37) & 0x10) != 0) {}

    kprintf("Software Reset Done\n");

    /* Set for receive buffer location */
    rx_buffer = kmalloc(RTL8139_RXBUFFER_SIZE_MALLOC);
    memset(rx_buffer, 0, RTL8139_RXBUFFER_SIZE_MALLOC);
    kprintf("rx_buffer: 0x%x, phys: 0x%x\n", rx_buffer, vitrual2phys(rx_buffer));
    outl(vitrual2phys(rx_buffer), io_addr + 0x30);

    /* Set for TOK and ROK */
    outw(0x0005, io_addr + 0x3C);

    /* Configuring receive buffer (RCR) */
    outl(0xf | (1 << 7), io_addr + 0x44); // (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP

    /* Enable Receive and Transmitter */
    outb(0x0C, io_addr + 0x37);

    /* Read the interrupt irq */
    uint32_t irq_num = (pci_read(RTL8139_BUS_ID, RTL8139_DEVICE_ID, 0, 0x3C) & 0xFF);
    kprintf("IRQ_NUM: 0x%x\n", irq_num);

    /* We suppose IRQ_NUM to be 0xB TODO: We should solve this later */
    if(irq_num != 0xB) {
        kprintf("IRQ_NUM ASSERT ERROR!!!\n");
        while(1) {}
    }

    read_mac_addr();

    /* Register Interrupt */
    enable_irq(irq_num);
}
