/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/13
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "terminal.h"
#include "paging.h"

volatile int32_t current_running_terminal;
volatile int32_t next_running_terminal;

/**
 * Init for scheduler that nothing is currently running
 */
void init_scheduler();

/* void schedule(); */

#endif //MP3_SCHEDULER_H
