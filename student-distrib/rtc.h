#ifndef _RTC_H
#define _RTC_H

#include "lib.h"
#include "types.h"
#include "x86_desc.h"
#include "i8259.h"

#define RTC_IRQ 0x8
#define RTC_PORT_SELECT 0x70
#define RTC_PORT_DATA 0x71

#define RTC_DEFAULT_RATE 0x06

#define RTC_REGISTER_A 0x8A
#define RTC_REGISTER_B 0x8B
#define RTC_REGISTER_C 0x0C


int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void *buf, int32_t nbytes);

/**
 * Interrupt handler for rtc, will be called in idtwrapper.S
 */
void rtc_handler();

/**
 * Init rtc and set rate to 0x0F
 */
void init_rtc();

#endif
