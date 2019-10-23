#include "rtc.h"

static uint32_t count = 0;

/**
 * Interrupt handler for rtc, will be called in idtwrapper.S
 */
void rtc_handler() {
    /* Status Register C will contain a bitmask telling which interrupt happened */
    /* We should read register C */
    outb(RTC_REGISTER_C, RTC_PORT_SELECT);
    inb(RTC_PORT_DATA);
    /* Call back to test RTC */
    //test_interrupts();
    /* Send the EOI */
    send_eoi(RTC_IRQ);

    count++;
}

/**
 * Init rtc and set rate to 0x0F
 * https://wiki.osdev.org/RTC
 */
void init_rtc() {
    /* First we try to turn on the RTC */
    char prev;
    outb(RTC_REGISTER_B, RTC_PORT_SELECT);  /* Select B */
    prev = inb(RTC_PORT_DATA);  /* Read prev data from B */
    outb(RTC_REGISTER_B, RTC_PORT_SELECT);  /* Reselect B */
    outb(prev | 0x40, RTC_PORT_DATA); /* Turn bit6 of B */

    /* Set for rate */
    int rate;
    rate = RTC_DEFAULT_RATE;
    outb(RTC_REGISTER_A, RTC_PORT_SELECT);
    prev = inb(RTC_PORT_DATA);
    outb(RTC_REGISTER_A, RTC_PORT_SELECT);
    outb((prev & 0xF0) | rate, RTC_PORT_DATA);

    enable_irq(RTC_IRQ);
}
