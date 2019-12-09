/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/7
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "dom.h"

inline RGBA_t* UIElementGetPixel(UIElement_t* element, uint32_t x, uint32_t y) {
    return &element->framebuffer[y * element->real_width + x];
}
inline void UIElementSetPixel(UIElement_t* element, uint32_t x, uint32_t y, uint32_t color, uint8_t force_set) {
    RGBA_t* pixel = UIElementGetPixel(element, x, y);
    if(element->is_transparent && !force_set) {
        pixel->a = 0;
    } else {
        pixel->r = ((color >> 16) & 0xFF);
        pixel->g = ((color >> 8) & 0xFF);
        pixel->b = (color & 0xFF);
        pixel->a = 0xFF;
    }
}
inline void UIElement_set_pixel(UIElement_t* element, uint32_t x, uint32_t y, RGBA_t* target_color) {
    RGBA_t* pixel = UIElementGetPixel(element, x, y);
    if(target_color->a == 0xFF) {
        pixel->r = target_color->r;
        pixel->g = target_color->g;
        pixel->b = target_color->b;
    }
}
inline void UIElement_set_margin(UIElement_t* element, uint32_t top, uint32_t right, uint32_t bottom, uint32_t left) {
    if(element == NULL)
        return;
    element->margin[0] = top;
    element->margin[1] = right;
    element->margin[2] = bottom;
    element->margin[3] = left;
}
inline void UIElement_set_padding(UIElement_t* element, uint32_t top, uint32_t right, uint32_t bottom, uint32_t left) {
    if(element == NULL)
        return;
    element->padding[0] = top;
    element->padding[1] = right;
    element->padding[2] = bottom;
    element->padding[3] = left;
}
inline void UIElement_clear_text(UIElement_t* element) {
    if(element->text_buffer_length > 0) {
        element->text_buffer_length = 0;
        kfree(element->text);
        element->text = NULL;
    }
}
inline void UIElement_set_text(UIElement_t* element, char* text, uint32_t color, uint8_t text_align) {
    uint32_t length = strlen(text);
    if(length + 1 > element->text_buffer_length) {
        if(element->text != NULL) {
            kfree(element->text);
        }
        element->text_buffer_length = length * 2;
        element->text = kmalloc(element->text_buffer_length);
    }

    element->text_align = text_align; /* 0 for default, 1 for vertical center, 2 for horz center */
    element->color = color;
    element->text = kmalloc(length + 1);
    memcpy(element->text, text, length);
    element->text[length] = '\0';
}
inline void UIElement_set_background(UIElement_t* element, uint8_t is_transparent, uint32_t color) {
    if(element == NULL)
        return;
    element->background = color;
    element->is_transparent = is_transparent;
}

void UIElement_append_child(UIElement_t* element, UIElement_t* child) {
    if(element == NULL || child == NULL)
        return;
    child->parent = element;
    if(element->children_head == NULL) {
        element->children_head = child;
        return;
    }
    UIElement_t* child_ptr;
    for(child_ptr = element->children_head; child_ptr->next != NULL; child_ptr = child_ptr->next) {}
    child_ptr->next = child;
    return;
}

void UIElement_copy_fb(UIElement_t* element, int32_t x_begin, int32_t y_begin, UIElement_t* child) {
    if(element == NULL || child == NULL)
        return;
    if(x_begin < 0 || y_begin < 0 || x_begin >= element->real_width || y_begin >= element->real_height)
        return;
    int32_t x = 0, y = 0;
    int32_t max_x = x_begin+child->real_width < element->real_width ? x_begin+child->real_width : element->real_width;
    int32_t max_y = y_begin+child->real_height < element->real_height ? y_begin+child->real_height : element->real_height;
    for(y = y_begin; y < max_y; y++) {
        for(x = x_begin; x < max_x; x++) {
            UIElement_set_pixel(element, x, y, UIElementGetPixel(child, x - x_begin, y - y_begin));
        }
    }
}

void UIElement_render_font(UIElement_t* element, int x_start, int y_start, char ch) {
    char* font = get_font(ch);
    int x,y;
    int set;
    for (y=0; y < 8; y++) {
        for (x=0; x < 8; x++) {
            set = font[y] & 1 << x;
            if(set) {
                UIElementSetPixel(element, x_start+x, y_start+y, element->color, 1);
            }
        }
    }
}

void UIElement_render_text(UIElement_t* element) {
    if(element->text != NULL) {
        int i, length = strlen(element->text);
        int cur_x = element->padding[3], cur_y = element->padding[0];
        if(element->text_align & UIE_TEXT_ALIGN_VERT_CENTER) {
            cur_y = element->padding[0] + ((element->real_height - element->padding[0] - element->padding[2])/2) - 4;
        }
        if(element->text_align & UIE_TEXT_ALIGN_HORZ_CENTER) {
            cur_x = element->padding[3] + ((element->real_width - element->padding[1] - element->padding[3])/2) - (length/2)*8;
        }
        for(i = 0; i < length; i++) {
            UIElement_render_font(element, cur_x, cur_y, element->text[i]);
            cur_x += 8;
            if(cur_x > element->real_width - element->padding[1]) {
                cur_x = element->padding[3];
                cur_y += 10; /* Not handle y overflow now */
                if(cur_y >= element->real_height)
                    return;
            }
        }
    }
}

void UIElement_render(UIElement_t* element) {
    if(element == NULL)
        return;
    return UIElement_render_responsive(element, element->width, element->height);
}

void UIElement_render_responsive(UIElement_t* element, int32_t real_width, int32_t real_height) {
    if(element == NULL)
        return;
    /* First we render the Element itself */


    /* To check whether we should allocate a new frame buffer */
    if(element->is_responsive || element->framebuffer == NULL) {
        element->real_width = real_width;
        element->real_height = real_height;
        if (element->framebuffer != NULL)
            kfree(element->framebuffer);
        element->framebuffer = kmalloc(sizeof(RGBA_t) * element->real_width * element->real_height);
    }

    /* First render the background */
    uint32_t x = 0, y = 0;
    for(x = 0; x < element->real_width; x++)
        for(y = 0; y < element->real_height; y++) {
            UIElementSetPixel(element, x, y, element->background, 0);
        }
    /* Then render fonts if we have some, render to the padding position by default */
    UIElement_render_text(element);



    UIElement_t* child_node;
    /* Process the responsive */
//    uint8_t should_one_vert_responsive = 0, should_one_horz_responsive = 0;
//    uint32_t responsive_height_cnt = 0, responsive_width_cnt;
//    for(child_node = element->children_head; child_node != NULL; child_node = child_node->next) {
//        if(child_node->height < 0)
//            responsive_height_cnt++;
//        if(child_node->width < 0)
//            responsive_width_cnt++;
//    }
//    if(responsive_height_cnt == 1)
//        should_one_vert_responsive = 1;
//    if(responsive_width_cnt == 1)
//        should_one_horz_responsive = 1;

    UIElement_t* special_responsive_child = NULL;
    int32_t total_height = element->padding[0] + element->padding[2];


    /* Then we render all the children */
    /* int32_t cur_y = element->padding[0], cur_x = element->padding[3]; */
    int32_t child_real_width = 0, child_real_height = 0;
    for(child_node = element->children_head; child_node != NULL; child_node = child_node->next) {
        if(child_node->is_responsive) {
            if(child_node->width <= 0 && child_node->height <= 0) {
                special_responsive_child = child_node;
                continue;
            }
            if(child_node->width <= 0) {
                child_real_width =  element->real_width - element->padding[1] - element->padding[3] - child_node->margin[1] - child_node->margin[3];
            } else {
                child_real_width = child_node->width;
            }
            if(child_node->height <= 0) {
                child_real_height = element->real_height - element->padding[0] - element->padding[2] - child_node->margin[0] - child_node->margin[2];
            } else {
                child_real_height = child_node->height;
            }
            UIElement_render_responsive(child_node, child_real_width, child_real_height);
            total_height += child_node->margin[0] + child_node->margin[2] + child_real_height;
        } else {
            UIElement_render_responsive(child_node, child_node->width, child_node->height);
            total_height += child_node->margin[0] + child_node->margin[2] + child_node->height;
        }

    }

    if(special_responsive_child != NULL) {
        child_real_width =  element->real_width - element->padding[1] - element->padding[3] - special_responsive_child->margin[1] - special_responsive_child->margin[3];
        child_real_height = element->real_height - total_height;
        UIElement_render_responsive(special_responsive_child, child_real_width, child_real_height);
    }

    /* Then we genrate the frame buffer */
    int32_t cur_y = element->padding[0], cur_x = element->padding[3];
    for(child_node = element->children_head; child_node != NULL; child_node = child_node->next) {
        cur_y += child_node->margin[0];
        UIElement_copy_fb(element, cur_x, cur_y, child_node);
        cur_y += child_node->real_height;
        cur_y += child_node->margin[2];
    }
}


UIElement_t* UIElement_allocate(int32_t width, int32_t height, uint8_t is_responsive) {
    UIElement_t* element = kmalloc(sizeof(UIElement_t));
    element->width = width;
    element->height = height;
    element->is_responsive = is_responsive;
    element->parent = NULL;
    element->direction = UIE_DIRECTION_VERT;
    element->text = NULL;
    element->text_buffer_length = 0;

    if(is_responsive) {
        element->framebuffer = NULL;
        element->real_width = 0;
        element->real_height = 0;
    } else {
        element->framebuffer = kmalloc(sizeof(RGBA_t) * width * height);
        element->real_width = width;
        element->real_height = height;
    }
    element->children_head = NULL;
    element->next = NULL;
    memset(element->padding, 0, sizeof(int32_t) * 4);
    memset(element->margin, 0, sizeof(int32_t) * 4);

    return element;
}
UIWindow_t* UIWindow_allocate(uint32_t x, uint32_t y, int32_t width, int32_t height, uint8_t is_root) {
    UIWindow_t* window = kmalloc(sizeof(UIWindow_t));
    window->is_root = is_root;
    window->wid = global_wid++;
    window->x = x;
    window->y = y;
    window->prev = NULL;
    window->next = NULL;
    window->element = UIElement_allocate(width, height, 0);
    window->should_render = 1;

    window->keyboard_event_handler = NULL;
    window->left_click_event_handler = NULL;
    window->left_release_event_handler = NULL;

    return window;
}

void UIWindow_append(UIWindow_t* window) {
    window->prev = window_tail;
    window->next = NULL;
    if(window_tail != NULL) {
        window_tail->next = window;
        window_tail = window;
    } else {
        window_head = window;
        window_tail = window;
    }
}
void UIWindow_attach_app(UIWindow_t* window, void* app) {
    window->app = app;
}
void UIWindow_prepend(UIWindow_t* window) {
    window->prev = NULL;
    window->next = window_head;
    if(window_head != NULL){
        window_head->prev = window;
        window_head = window;
    } else {
        window_head = window;
        window_tail = window;
    }
}
UIWindow_t* UIWindow_match_window(uint32_t x, uint32_t y) {
    UIWindow_t* window;
    for(window = window_head; window != NULL; window = window->next) {
        if(x >= window->x && x < window->x+window->element->real_width && y >= window->y && y < window->y +window->element->real_height) {
            return window;
        }
    }
    return NULL;
}

void UIWindow_remove(UIWindow_t* window) {
    if(window->prev != NULL) {
        (window->prev)->next = window->next;
    } else {
        window_head = window->next;
    }
    if(window->next != NULL) {
        (window->next)->prev = window->prev;
    } else {
        window_tail = window->prev;
    }
}
void UIWindow_shift_top(UIWindow_t* window) {
    UIWindow_remove(window);
    UIWindow_prepend(window);
}
void UIWindow_render(UIWindow_t* window) {
    if(window == NULL)
        return;
    uint32_t x, y;
    for(y = 0; y < window->element->real_height; y++) {
        for(x = 0; x < window->element->real_width; x++){
            draw_pixel_fb(window->x + x, window->y + y, UIElementGetPixel(window->element, x, y));
        }
    }
}
void UIWindow_list_render() {
    UIWindow_t* window;
    for(window = window_tail; window != NULL; window = window->prev) {
        if(window->should_render) {
            UIElement_render(window->element);
            window->should_render = 0;
        }
        UIWindow_render(window);
    }
}
void init_UIWindow() {
    window_head = NULL;
    window_tail = NULL;
    global_wid = 0;
}
