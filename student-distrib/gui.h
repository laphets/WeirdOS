/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/19
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _GUI_H
#define _GUI_H

#include "vga.h"

/* To indicate whether the gui is enabled */
int gui_enabled;

void render_window(int x, int y, int width, int height, char* title, uint8_t is_focus);

void render_cursor(int x, int y);

#endif //MP3_GUI_H
