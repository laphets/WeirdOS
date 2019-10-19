#ifndef _RTC_H
#define _RTC_H

#include "lib.h"
#include "types.h"
#include "x86_desc.h"
#include "i8259.h"

#define RTC_IRQ 0x8
#define RTC_PORT_SELECT 0x70
#define RTC_PORT_DATA 0x71

#define RTC_REGISTER_A 0x8A
#define RTC_REGISTER_B 0x8B
#define RTC_REGISTER_C 0x0C

void rtc_handler();
void init_rtc();

#endif //MP3_GROUP_42_RTC_H
