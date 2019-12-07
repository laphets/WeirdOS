/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/19
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _MOUSE_H
#define _MOUSE_H

#include "lib.h"

#define MOUSE_IRQ 12

#define MOUSE_TIMEOUT 100000
#define MOUSE_WAIT_TYPE_DATA 0
#define MOUSE_WAIT_TYPE_SIGNAL 1
#define MOUSE_CTRL_PORT 0x64
#define MOUSE_DATA_PORT 0x60

void init_mouse();
void mouse_wait(uint8_t type);
void mouse_write(uint8_t data);
uint8_t mouse_read();
void mouse_handler();

int32_t mouse_x;
int32_t mouse_y;

#endif //MP3_MOUSE_H
