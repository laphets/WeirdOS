/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/19
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _GUI_H
#define _GUI_H

#include "vga.h"
#include "dom.h"
#include "mouse.h"

int gui_enabled;
uint32_t current_fps;

#define DEFAULT_FPS 20

void init_gui();

void gui_error(char* string);
void gui_debug(char* string);

void render_window(int x, int y, int width, int height, char* title, uint8_t is_focus);

void render_cursor(int x, int y);
void render_screen();

void handle_left_click_event();
void handle_left_release_event();
void handle_right_click_event();
void handle_right_release_event();
void hanlde_keyboard_event(char ascii);

void* gui_render_application_window(void* window, char* title);

#endif //MP3_GUI_H
