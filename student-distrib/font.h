/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/19
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _FONT_H
#define _FONT_H

#include "vga.h"

void render_font(int x_start, int y_start, char ch, uint32_t color);

void render_string(int x_start, int y_start, char* string, uint32_t color);

inline char* get_font(char ch);

#endif //MP3_FONT_H
