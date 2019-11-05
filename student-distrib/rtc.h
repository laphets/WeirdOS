#ifndef _RTC_H
#define _RTC_H

#include "lib.h"
#include "types.h"
#include "x86_desc.h"
#include "i8259.h"
#include "vfs.h"


#define RTC_IRQ 0x8
#define RTC_PORT_SELECT 0x70
#define RTC_PORT_DATA 0x71

#define NUM_RTC_VALUES 16
#define RTC_DEFAULT_RATE 0x06

#define RTC_REGISTER_A 0x8A
#define RTC_REGISTER_B 0x8B
#define RTC_REGISTER_C 0x0C

#define OPEN_RTC_RATE 0xF
#define MAX_RTC_HZ 1024
#define LOW_4_BITS 0xF

#define INT_BYTES_SIZE 4

/* Updates the clock to 2 Hz when the RTC is opened */
int32_t rtc_open(const uint8_t* filename);

/* Has no function */
int32_t rtc_close(int32_t fd);

/* Sets the wait flag to 1 & waits until the RTC interrupt to clear the flag before returning */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);

/* Takes in a Hz clock rate and updates the RTC */
int32_t rtc_write(int32_t fd, const void *buf, int32_t nbytes);

/* Interrupt handler for rtc, will be called in idtwrapper.S */
void rtc_handler();

/* Init rtc and set rate to 0x0F */
void init_rtc();

file_operator_t rtc_operator;

void init_rtc_operator();

/* hertzmap[16]:
 * Holds all the possible Hertz values of the RTC
 * Values greater than 1024 should not be used as 
 *      generating more than 1000 interrupts a second 
 *      will cause significant performance decrease
 * Since 0x0 is no output, it is represented as NULL
 */
extern uint32_t hertzmap[NUM_RTC_VALUES];

#endif
