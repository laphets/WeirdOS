#include "rtc.h"
#include "i8259.h"

volatile static uint32_t count = 0;
volatile char wait = 0;
uint32_t hertzmap[16] = {NULL, 256, 128, 8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2};

/* int32_t rtc_open(const uint8_t *filename);
 * Description: Updates the clock to 2 Hz when 
 *              the RTC is opened
 * Input: filename -- Unused inside the function
 * Output: Returns 0 on success
 * Side effect: Updates the clock to 2 Hz 
 */
int32_t rtc_open(const uint8_t *filename) {
    int rate = OPEN_RTC_RATE;
    char prev;
    /* Read value in previous in register A */
    outb(RTC_REGISTER_A, RTC_PORT_SELECT);
    prev = inb(RTC_PORT_DATA);
    /* Enable  Update in Progress & Set rate to 2 Hz*/
    outb(RTC_REGISTER_A, RTC_PORT_SELECT);
    outb((prev & 0xF0) | rate, RTC_PORT_DATA);

    count = 0;
    return 0;
}

/* int32_t rtc_close(int32_t fd);
 * Description: Has no function
 * Input: fd -- Unused inside the function
 * Output: Returns 0 on success
 * Side effect: none
 */
int32_t rtc_close(int32_t fd) {
    return 0;
}

/* int32_t rtc_read(int32_t fd, void *buf, int32_t nbytes);
 * Description: Sets the wait flag to 1 & waits until 
 *              the RTC interrupt to clear the flag 
 *              before returning
 * Input: fd -- Unused inside the function
 *        buf -- Unused inside the function
 *        nbytes -- Unused inside the function
 * Output: none
 * Side effect: Waits until next RTC interrupt before returning
 */
int32_t rtc_read(int32_t fd, void *buf, int32_t nbytes) {
wait = 1;
while(wait){
    continue;
}
return 0;
}

/* int32_t rtc_write(int32_t fd, const void *buf, int32_t nbytes);
 * Description: Takes in a Hz clock rate and updates the RTC
 * Input: fd -- Unused inside the function
 *        buf -- Pointer to an int with the Hz clock rate
 *        nbytes -- Should always be 4 in a valid write
 * Output: Returns 0 on successful RTC update else fail
 * Side effect: Updates the RTC clock rate
 */
int32_t rtc_write(int32_t fd, const void *buf, int32_t nbytes) {
    unsigned int hertz, i;
    unsigned char rate, prev;
    /* Check for valid inputs */
    if(!buf | (nbytes != 4)){
        return -1;
    }
    /* Get hertz value from buffer */
    hertz = *((int*) buf);
    if(hertz > MAX_RTC_HZ){
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

    /* Clears old rate and places new one in Register A */
    outb(RTC_REGISTER_A, RTC_PORT_SELECT);
    prev = inb(RTC_PORT_DATA);
    outb(RTC_REGISTER_A, RTC_PORT_SELECT);
    outb((prev & 0xF0) | rate, RTC_PORT_DATA);

    count = 0;
    return 0;
}

/* void rtc_handler();
 * Description: Reads which interrupt happened, calls interrupt function,
 *              & sends eoi when finished. Called by idtwrapper.S
 * Input: none
 * Output: none
 * Side effect: Resets the wait flag & increments count
 */
void rtc_handler() {
    /* Status Register C will contain a bitmask telling which interrupt happened */
    /* We should read register C */
    outb(RTC_REGISTER_C, RTC_PORT_SELECT);
    inb(RTC_PORT_DATA);
    /* Call back to test RTC */
    test_interrupts(count);
    /* Send the EOI */
    send_eoi(RTC_IRQ);
    wait = 0;
	// printf("RTC Interrupt\n");
    count++;
}

/**
 * Init rtc and set rate to 0x0F
 * https://wiki.osdev.org/RTC
 */

/* void init_rtc();
 * Description: Reads which interrupt happened, calls interrupt function,
 *              & sends eoi when finished
 * Input: none
 * Output: none
 * Side effect: Initializes the RTC to 1024 Hz
 */
void init_rtc() {
    /* First we try to turn on the RTC */
    char prev;
    outb(RTC_REGISTER_B, RTC_PORT_SELECT);  /* Select B */
    prev = inb(RTC_PORT_DATA);  /* Read prev data from B */
    outb(RTC_REGISTER_B, RTC_PORT_SELECT);  /* Reselect B */
    outb(prev | 0x40, RTC_PORT_DATA); /* Turn Periodic Interrupt Enable (bit 6) in B */

    /* Set for rate */
    int rate;
    rate = RTC_DEFAULT_RATE;

    /* Clears old rate and places new one in Register A */
    outb(RTC_REGISTER_A, RTC_PORT_SELECT);
    prev = inb(RTC_PORT_DATA);
    outb(RTC_REGISTER_A, RTC_PORT_SELECT);
    outb((prev & 0xF0) | rate, RTC_PORT_DATA); 

    enable_irq(RTC_IRQ);

    count = 0;
}
