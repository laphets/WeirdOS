/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/14
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _PIT_H
#define _PIT_H

#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "scheduler.h"

#define PIT_IRQ 0x0
#define PIT_DEFAULT_FREQ 50
#define PIT_CMD_PORT 0x43
#define PIT_CHAN0_PORT 0x40
#define PIT_TIMER_DIVIDER 1193180   /* 1.19MHz */
#define PIT_INIT_CMD 0x36 /* Square Wave */

void init_pit();
void pit_handler();
void pit_set_freq(int hz);

#endif //_PIT_H
