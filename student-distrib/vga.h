/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/18
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _VGA_H
#define _VGA_H

#include "lib.h"
#include "vbe.h"
#include "font.h"
#include "gui.h"

/*
 * IMAGE  is the whole screen in mode X: 320x200 pixels in our flavor.
 * SCROLL is the scrolling region of the screen.
 *
 * X_DIM   is a horizontal screen dimension in pixels.
 * X_WIDTH is a horizontal screen dimension in 'natural' units
 *         (addresses, characters of text, etc.)
 * Y_DIM   is a vertical screen dimension in pixels.
 */
#define SVGA_X_DIM 1024
#define SVGA_Y_DIM 768

volatile int screen_changed;

typedef struct RGBA {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
} RGBA_t;

void init_vga();

void draw_pixel(int x, int y, uint32_t color);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void show_screen();


#endif //MP3_VGA_H
