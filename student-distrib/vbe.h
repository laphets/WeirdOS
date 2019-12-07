/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/19
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _VBE_H
#define _VBE_H

#include "lib.h"
#include "types.h"

#define VBE_ADDR 0xFD000000

void vbe_set(uint16_t xres, uint16_t yres, uint16_t bpp);

#endif //MP3_VBE_H
