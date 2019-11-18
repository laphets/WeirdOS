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

void init_pit();
void pit_handler();
void pit_set_freq(int hz);

#endif //_PIT_H
