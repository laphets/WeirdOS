/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/19
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "gui.h"


void render_window(int x, int y, int width, int height, char* title, uint8_t is_focus) {
    draw_rect(x, y, width, height, 0xCDCECF);
    if(is_focus) {
        draw_rect(x+2, y+2, width-4, 20, 0x000E8C);
        render_string(x+16, y+8, title, 0xFFFFFF);
    } else {
        draw_rect(x+2, y+2, width-4, 20, 0x929292);
        render_string(x+16, y+8, title, 0xC7C7C7);
    }

}

void render_cursor(int x, int y) {
    static char cursor[16][16] = {
            "**************..",
            "*OOOOOOOOOOO*...",
            "*OOOOOOOOOO*....",
            "*OOOOOOOOO*.....",
            "*OOOOOOOO*......",
            "*OOOOOOO*.......",
            "*OOOOOOO*.......",
            "*OOOOOOOO*......",
            "*OOOO**OOO*.....",
            "*OOO*..*OOO*....",
            "*OO*....*OOO*...",
            "*O*......*OOO*..",
            "**........*OOO*.",
            "*..........*OOO*",
            "............*OO*",
            ".............***"
    };
    int xd, yd;
    for(yd = 0; yd < 16; yd++)
        for(xd = 0; xd < 16; xd++) {
            if(cursor[yd][xd] == '*')
                draw_pixel(x+xd, y+yd, 0);
            else if(cursor[yd][xd] == '0')
                draw_pixel(x+xd, y+yd, 0xFFFFFF);
        }
}