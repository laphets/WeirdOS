/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/19
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "gui.h"

UIWindow_t* clicked_window;
UIWindow_t* focused_window;
UIWindow_t* background;
UIWindow_t* debug_view;
UIWindow_t* desktop_status_bar;
int32_t clicked_window_x_offset, clicked_window_y_offset;

void gui_init_background() {
    background = UIWindow_allocate(0 ,0, SVGA_X_DIM, SVGA_Y_DIM, 1);
    background->element->background = 0x6AA8A9;
    UIElement_render(background->element);
}

void gui_init_desktop_status_bar() {
    desktop_status_bar = UIWindow_allocate(0, SVGA_Y_DIM - 30, SVGA_X_DIM, 30, 1);
    desktop_status_bar->element->background = 0xC4C7CB;
    UIElement_render(desktop_status_bar->element);
}

void gui_error(char* string) {
    draw_rect(0, 0, SVGA_X_DIM, SVGA_Y_DIM, 0x0224EE);
    render_string(10, 10, string, 0xFFFFFF);
    show_screen();
}

void gui_debug(char* string) {
    if(gui_enabled && debug_view != NULL) {
        uint8_t* new_string = kmalloc(strlen(string) + debug_view->content->text_buffer_length);
        sprintf(new_string, "%s\n%s", debug_view->content->text, string);
        UIElement_set_text(debug_view->content, (char*)new_string, 0xFFFFFF, UIE_TEXT_ALIGN_DEFAULT);

        debug_view->should_render = 1;
        screen_changed = 1;
    }
}

void gui_init_debug_view() {
    debug_view = UIWindow_allocate(0, 0, 500, 300, 1);
    gui_render_application_window(debug_view, "Debug Console");
    UIElement_set_background(debug_view->content, 0, 0x000000);
    UIElement_set_text(debug_view->content, "ECE391 OS Debug Console Started...", 0xFFFFFF, UIE_TEXT_ALIGN_DEFAULT);
    UIElement_set_padding(debug_view->content, 4, 4, 4, 4);

    UIWindow_append(debug_view);
}

void gui_render_close_btn(UIElement_t* parent) {
    /* Close Btn */
    int32_t width = 10, height = 10;
    UIElement_t* close_btn = UIElement_allocate(width, height, 0, 1);
    close_btn->framebuffer = kmalloc(sizeof(RGBA_t) * width * height);
    close_btn->position = UIE_POSITION_ABSOLUTE;
    close_btn->right = 14;
    close_btn->top = 3;
    static char closebtn[10][10] = {
            "QQQQQQQQQQ",
            "Q@@QQQQ@@Q",
            "QQ@@QQ@@QQ",
            "QQQ@@@@QQQ",
            "QQQQ@@QQQQ",
            "QQQ@@@@QQQ",
            "QQ@@QQ@@QQ",
            "Q@@QQQQ@@Q",
            "QQQQQQQQQQ",
            "QQQQQQQQQQ"
    };
    uint32_t x = 0, y = 0;
    for(y = 0; y < height; y++) {
        for(x = 0; x < width; x++) {
            if(closebtn[y][x] == 'Q') {
                UIElementSetPixel(close_btn, x, y, 0xC6C6C6, 1);
            } else {
                UIElementSetPixel(close_btn, x, y, 0x000000, 1);
            }
        }
    }

    /* UIElement_set_text(close_btn, "close", 0x000000, UIE_TEXT_ALIGN_VERT_CENTER | UIE_TEXT_ALIGN_HORZ_CENTER); */
    UIElement_append_child(parent, close_btn);
}

void* gui_render_application_window(void* window_, char* title) {
    UIWindow_t* window = window_;
    if(window == NULL || !window->is_root)
        return NULL;

    window->element->background = 0xC4C7CB;
    window->element->direction = UIE_DIRECTION_VERT;
    UIElement_set_padding(window->element, 4, 4, 0, 4);

    /* Title Bar */
    UIElement_t* title_bar = UIElement_allocate(-1, 16, 1, 0);
    if(window_head == NULL) {
        UIElement_set_background(title_bar, 0, 0x0015A3);
    } else {
        UIElement_set_background(title_bar, 0, 0x919294);
    }
    UIElement_set_padding(title_bar, 2, 2, 2, 5);
    UIElement_set_text(title_bar, title, 0xFFFFFF, UIE_TEXT_ALIGN_VERT_CENTER);
    UIElement_append_child(window->element, title_bar);
    window->title_bar = title_bar;

    gui_render_close_btn(title_bar);


    /* Menu Bar */
    UIElement_t* menu_bar = UIElement_allocate(-1, 16, 1, 0);
    UIElement_set_background(menu_bar, 1, 0xFF00FF);
    UIElement_set_padding(menu_bar, 2, 2, 2, 5);
    UIElement_set_text(menu_bar, "File Edit View Help", 0x000000, UIE_TEXT_ALIGN_VERT_CENTER);
    UIElement_append_child(window->element, menu_bar);

    /* The content */
    UIElement_t* content = UIElement_allocate(-1, -1, 1, 0);
    UIElement_set_background(content, 0, 0xFFFFFF);
    UIElement_append_child(window->element, content);
    content->enable_scroll_view = 1;
    window->content = content;

    /* The status bar */
    UIElement_t* status_bar = UIElement_allocate(-1, 16, 1, 0);
    UIElement_set_background(status_bar, 1, 0xFFAAFF);
    UIElement_set_padding(status_bar, 4, 0, 4, 5);
    UIElement_set_text(status_bar, "Ready", 0x000000, UIE_TEXT_ALIGN_VERT_CENTER);
    UIElement_append_child(window->element, status_bar);

    /* Trigger to render that UI Element */
    UIElement_render(window->element);

    return content;
}

void gui_init_windows() {
    clicked_window = NULL;
    focused_window = NULL;
    debug_view = NULL;
//    int i = 0;
//    for(i = 0; i < 4; i++) {
//        char title[30];
//        sprintf(title, "Window %d", i);
//        UIWindow_t* window = UIWindow_allocate(350 + 50*i, 240 + 50*i, 500, 350, 1);
//        gui_render_application_window(window, title);
//        UIWindow_append(window);
//    }
}

void de_focus_window(UIWindow_t* window) {
    UIElement_set_background(window->title_bar, 0, 0x919294);
    window->should_render = 1;
}
void set_focus_window(UIWindow_t* window) {
    UIElement_set_background(window->title_bar, 0, 0x0015A3);
    window->should_render = 1;
}

void handle_left_click_event() {
    kprintf("mouse_left_click_event\n");
    clicked_window = UIWindow_match_window(mouse_x, mouse_y);
    if(clicked_window != NULL) {
        de_focus_window(focused_window);
        focused_window = clicked_window;
        set_focus_window(focused_window);
    }

    if(clicked_window != NULL) {
        clicked_window_x_offset = mouse_x - ((UIWindow_t*)clicked_window)->x;
        clicked_window_y_offset = mouse_y - ((UIWindow_t*)clicked_window)->y;
        UIWindow_shift_top(clicked_window);
    }
}

void handle_left_release_event() {
    kprintf("mouse_left_release_event\n");
    clicked_window = NULL;

}

void handle_right_click_event() {
    kprintf("mouse_right_click_event\n");


}

void handle_right_release_event() {
    kprintf("mouse_right_release_event\n");

}

/**
 *
 * @param type 0 for up, 1 for down
 */
void handle_keyboard_scroll(uint8_t type) {
    UIElement_t* content = NULL;
    if(focused_window == NULL || focused_window->content == NULL)
        return;
    content = focused_window->content;
    if(focused_window->app_content != NULL && focused_window->app_content->enable_scroll_view)
        content = focused_window->app_content;

    if(type == 1) {
        /* Up */
        content->scroll_y_offset += 12;
    } else {
        /* Down */
        content->scroll_y_offset -= 12;
        if(content->scroll_y_offset < 0)
            content->scroll_y_offset = 0;
    }
    focused_window->should_render = 1;
    screen_changed = 1;
}

void hanlde_keyboard_event(char ascii) {
    if(focused_window != NULL) {
        focused_window->keyboard_event_handler(focused_window->app, ascii);
    }
}

void render_screen() {
    if(screen_changed == 0)
        return;
    screen_changed = 0;


    if(clicked_window != NULL) {
        ((UIWindow_t*)clicked_window)->x = mouse_x - clicked_window_x_offset;
        ((UIWindow_t*)clicked_window)->y = mouse_y - clicked_window_y_offset;
    }


    /* Firstly, render the background */
    UIWindow_render(background);
    /* Then render other windows */
    UIWindow_list_render();
    /* Render status bar */
    UIWindow_render(desktop_status_bar);
    /* Finally render the cursor */
    render_cursor(mouse_x, mouse_y);

    /* Set for the focus window */
    focused_window = window_head;

    /* Then show the screen */
    show_screen();
}

void init_gui() {
    current_fps = DEFAULT_FPS;
    gui_enabled = 1;

    gui_init_background();
    gui_init_windows();
    gui_init_desktop_status_bar();
    gui_init_debug_view();;

    screen_changed = 1;
    render_screen();
}



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

/**
 * Render cursor to some certain place
 * @param x
 * @param y
 */
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
            else if(cursor[yd][xd] == 'O')
                draw_pixel(x+xd, y+yd, 0xFFFFFF);
        }
}
