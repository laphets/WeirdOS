/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/18
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "vga.h"

RGBA_t* vbe_mem = (RGBA_t*)VBE_ADDR;
static RGBA_t framebuffer[SVGA_Y_DIM][SVGA_X_DIM] __attribute__((aligned(32)));

/* For 4 bytes */
static void copy_framebuffer(void* fb, void* vm_addr, int num_bytes) {
    asm volatile ("                                             \n\
        cld                                                     \n\
        rep movsl    /* copy ECX bytes from M[ESI] to M[EDI] */ \n\
        "
    : /* no outputs */
    : "S"(fb), "D"(vm_addr), "c"(num_bytes)
    : "eax", "memory"
    );
}

/*
 * clear_screens
 *   DESCRIPTION: Fills the video memory with zeroes.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: fills all 256kB of VGA video memory with zeroes
 */
void
clear_screens ()
{
    draw_rect(0, 0, SVGA_X_DIM, SVGA_Y_DIM, 0);
    show_screen();
}

/*
 * show_screen
 *   DESCRIPTION: Show the logical view window on the video display.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: copies from the build buffer to video memory;
 *                 shifts the VGA display source to point to the new image
 */
void show_screen() {
    if (gui_enabled == 1 && screen_changed == 1) {
        screen_changed = 0;
        copy_framebuffer(framebuffer, vbe_mem, SVGA_Y_DIM * SVGA_X_DIM);
    }
}


void init_vga() {
    gui_enabled = 1;
    vbe_set(SVGA_X_DIM, SVGA_Y_DIM, 32);

    draw_rect(0, 0, SVGA_X_DIM, SVGA_Y_DIM, 0x3E9092);

    render_window(100, 100, 600, 400, "Welcome to ECE391", 1);
    int i = 0;
    for(i = 0; i < 10; i++)
        render_window(350 + 20*i, 540 + 10*i, 200, 100, "Hello World", 0);

    render_cursor(200, 200);
}

/**
 * Draw a rect
 * @param x0
 * @param y0
 * @param x1
 * @param y1
 * @param color should be rgb
 */
void draw_rect(int x, int y, int width, int height, uint32_t color) {
    int i, j;
    for(i = 0; i < width; i++)
        for(j = 0; j < height; j++)
            draw_pixel(x+i, y+j, color);
}

void draw_pixel(int x, int y, uint32_t color) {
    if(x < 0 || y < 0 || x >= SVGA_X_DIM || y >= SVGA_Y_DIM || gui_enabled == 0)
        return;
    screen_changed = 1;
    framebuffer[y][x].r = ((color >> 16) & 0xFF);
    framebuffer[y][x].g = ((color >> 8) & 0xFF);
    framebuffer[y][x].b = (color & 0xFF);
}
