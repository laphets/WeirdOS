/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/7
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _DOM_H
#define _DOM_H

#include "lib.h"
#include "vga.h"

#define UIE_MAX_CHILD_NUM 10

#define UIE_DIRECTION_VERT 0
#define UIE_DIRECTION_HORZ 1

#define UIE_TEXT_ALIGN_DEFAULT (0)
#define UIE_TEXT_ALIGN_VERT_CENTER (1 << 0)
#define UIE_TEXT_ALIGN_HORZ_CENTER (1 << 1)

#define UIE_POSITION_RELATIVE 0
#define UIE_POSITION_ABSOLUTE 1

#define UIE_ALIGN_START 0
#define UIE_ALIGN_END 1
#define UIE_ALIGN_CENTER 2

typedef struct UIElement {
    /* The link list implementation */
    struct UIElement* children_head;
    struct UIElement* prev;
    struct UIElement* next;
    struct UIElement* parent;

    /* The style implementation */
    uint8_t position;
    uint8_t is_manual_render;
    int32_t top, right, bottom, left;

    uint8_t direction;
    uint8_t align;
    int32_t margin[4];
    int32_t padding[4];
    uint32_t background;
    uint8_t is_transparent;
    char* text;
    uint32_t text_buffer_length;
    uint8_t text_align;
    uint32_t color;
    uint8_t is_responsive;
    int32_t height, width;
    int32_t real_height, real_width;    /* Real time calculated */
    struct RGBA* framebuffer;
} UIElement_t;

inline void UIElementSetPixel(UIElement_t* element, uint32_t x, uint32_t y, uint32_t color, uint8_t force_set);
void UIElement_set_padding(UIElement_t* element, uint32_t top, uint32_t right, uint32_t bottom, uint32_t left);
inline void UIElement_set_margin(UIElement_t* element, uint32_t top, uint32_t right, uint32_t bottom, uint32_t left);
inline void UIElement_clear_text(UIElement_t* element);
inline void UIElement_set_text(UIElement_t* element, char* text, uint32_t color, uint8_t text_align);
void UIElement_append_child(UIElement_t* element, UIElement_t* child);
void UIElement_set_background(UIElement_t* element, uint8_t is_transparent, uint32_t color);
void UIElement_render(UIElement_t* element);
void UIElement_render_responsive(UIElement_t* element, int32_t real_width, int32_t real_height);

typedef void (*keyboard_event_handler_t)(void* app, char ascii);

typedef struct UIWindow {
    uint32_t wid;
    uint8_t is_root;
    struct UIWindow* next;
    struct UIWindow* prev;
    UIElement_t* element;
    int32_t x, y;

    int8_t should_render;

    void* app;

    void* left_click_event_handler;
    void* left_release_event_handler;
    keyboard_event_handler_t keyboard_event_handler;

    UIElement_t* title_bar;
    UIElement_t* content;
} UIWindow_t;

UIWindow_t* window_head;
UIWindow_t* window_tail;

uint32_t global_wid;

UIElement_t* UIElement_allocate(int32_t width, int32_t height, uint8_t is_responsive, uint8_t is_manual_render);
UIWindow_t* UIWindow_allocate(uint32_t x, uint32_t y, int32_t width, int32_t height, uint8_t is_root);

void UIWindow_append(UIWindow_t* window);
void UIWindow_prepend(UIWindow_t* window);
void UIWindow_shift_top(UIWindow_t* window);
void init_UIWindow();
UIWindow_t* UIWindow_match_window(uint32_t x, uint32_t y);

void UIWindow_render(UIWindow_t* window);
void UIWindow_list_render();

void UIWindow_attach_app(UIWindow_t* window, void* app);

#endif //MP3_DOM_H
