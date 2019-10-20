#include "rtc.h"
#include "i8259.h"

static uint32_t count = 0;

void rtc_handler() {
    /* Status Register C will contain a bitmask telling which interrupt happened */
    /* We should read register C */
    outb(RTC_REGISTER_C, RTC_PORT_SELECT);
    inb(RTC_PORT_DATA);
    /* Send the EOI */
    send_eoi(RTC_IRQ);

    count++;

    printf("RTC comes!!! %d \n", count);
}

/**
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
    rate = 0x0F;
    outb(RTC_REGISTER_A, RTC_PORT_SELECT);
    prev = inb(RTC_PORT_DATA);
    outb(RTC_REGISTER_A, RTC_PORT_SELECT);
    outb((prev & 0xF0) | rate, RTC_PORT_DATA);

    enable_irq(RTC_IRQ);
}
