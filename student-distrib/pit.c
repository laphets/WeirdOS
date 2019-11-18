/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/14
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 * Ref: http://www.osdever.net/bkerndev/Docs/pit.htm
 */
#include "pit.h"

/**
 * Hanlder for pit interrupt
 */
void pit_handler() {
    /* printf("pit interrupt comes...\n"); */
    send_eoi(PIT_IRQ);
    cli();
    /* We will trigger schedule here */
    switch_terminal(0, TYPE_SWITCH_RUNNING_TERMINAL);
}

/**
 * Init for pit, and set for the frequency
 */
void init_pit() {
    pit_set_freq(PIT_DEFAULT_FREQ);   /* Every 20ms raise an interrupt for scheduler */
    enable_irq(PIT_IRQ);
}

/**
 * Set the interval freq for pit
 * @param hz freq we want to set in hz
 */
void pit_set_freq(int hz) {
    int32_t divisor = PIT_TIMER_DIVIDER / hz;
    outb(PIT_INIT_CMD, PIT_CMD_PORT);
    outb(divisor & 0xFF, PIT_CHAN0_PORT); /* Get the lower 8 bit */
    outb(divisor >> 8, PIT_CHAN0_PORT);   /* Get the higher 8 bit */
}
