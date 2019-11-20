/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/14
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "pit.h"
/* http://www.osdever.net/bkerndev/Docs/pit.htm */

/**
 * Hanlder for pit interrupt
 */
void pit_handler() {
    /* printf("pit interrupt comes...\n"); */
    send_eoi(PIT_IRQ);
    cli();
    /* We will trigger schedule here */
    switch_terminal(0, 1);
    sti();
}

/**
 * Init for pit, and set for the frequency
 */
void init_pit() {
    pit_set_freq(50);   /* Every 20ms raise an interrupt for scheduler */
    enable_irq(PIT_IRQ);
}

/**
 * Set the interval freq for pit
 * @param hz freq we want to set in hz
 */
void pit_set_freq(int hz) {
    int32_t divisor = 1193180 / hz;
    outb(0x36, 0x43);
    outb(divisor & 0xFF, 0x40);
    outb(divisor >> 8, 0x40);
}
