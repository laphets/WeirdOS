#include "rtc.h"
#include "i8259.h"

static uint32_t count = 0;
volatile char wait = 0;
unsigned int rate = 0;

/* int32_t open(const uint8_t *filename);
 * Input: 
 * Output:
 * Function: 
 */
int32_t open(const uint8_t *filename) {
    int rate = 15;
    char prev;
    // dissable interrupt -------------------------
    // printf("pre irq disable\n");
    local_irq_save();
    // printf("post irq disable\n");
    outb(RTC_REGISTER_A, RTC_PORT_SELECT);
    prev = inb(RTC_PORT_DATA);
    outb(RTC_REGISTER_A, RTC_PORT_SELECT);
    outb((prev & 0xF0) | rate, RTC_PORT_DATA);
    // printf("pre irq enable\n");
    local_irq_restore();
    // printf("post irq enable\n");
    // enable interrupt -------------------------
    return 0;
}

int32_t close(int32_t fd) {
    return 0;
}

int32_t read(int32_t fd, void *buf, int32_t nbytes) {
wait = 1;
while(wait){
    continue;
}
return 0;
}

int32_t write(int32_t fd, const void *buf, int32_t nbytes) {
    unsigned int hertz, i;
    unsigned char rate, prev;
    /* Sanit checks */
    if((!buf | nbytes) != 4){
        return -1;
    }
    /* Get hertz value from buffer */
    hertz = *((int*) buf);
    if(hertz > MAX_RTC_RATE){
        return -1;
    }
    /* Check if hertz value is a power of two */
    for(i = 1; i < 16; i++){
        if(hertzmap[i] == hertz){
            break;
        }
    }
    if(i == 16){ /* Not a power of two*/
        return -1;
    }
    /* i is now the 4 bits of the rate */
    rate = i & LOW_4_BITS;

    /* Update  */
    local_irq_save();
    outb(RTC_REGISTER_A, RTC_PORT_SELECT);
    prev = inb(RTC_PORT_DATA);
    outb(RTC_REGISTER_A, RTC_PORT_SELECT);
    outb((prev & 0xF0) | rate, RTC_PORT_DATA);
    local_irq_restore();
    return 0;
}


/**
 * Interrupt handler for rtc, will be called in idtwrapper.S
 */
void rtc_handler() {
    /* Status Register C will contain a bitmask telling which interrupt happened */
    /* We should read register C */
    outb(RTC_REGISTER_C, RTC_PORT_SELECT);
    inb(RTC_PORT_DATA);
    /* Call back to test RTC */
    test_interrupts();
    /* Send the EOI */
    send_eoi(RTC_IRQ);
    wait = 0;
	printf("RTC Interrupt\n");
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
