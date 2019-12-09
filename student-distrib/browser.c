/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/8
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "browser.h"

static void left_click_event_handler() {

}
static void left_release_event_handler() {

}
static void keyboard_event_handler(browser_t* browser, char ch) {
    char* updated_text = kmalloc(strlen(browser->addr_bar->text) + 5);
    sprintf(updated_text, "%s%c", browser->addr_bar->text, ch);
    UIElement_set_text(browser->addr_bar, updated_text, 0x000000, UIE_TEXT_ALIGN_VERT_CENTER);
    browser->app->window->should_render = 1;
    screen_changed = 1;
}

void launch_browser() {
    browser_t* browser = kmalloc(sizeof(browser_t));
    browser->app = new_app("ECE391 Internet Explorer");

    /* Attch browser itself */
    UIWindow_attach_app(browser->app->window, browser);

    /* Init all the handlers */
    browser->app->window->keyboard_event_handler = keyboard_event_handler;
    browser->app->window->left_click_event_handler = left_click_event_handler;
    browser->app->window->left_release_event_handler = left_release_event_handler;

    /* Draw the layout */
    UIElement_set_background(browser->app->canvas, 0, 0xC4C7CB);
    /* First add the address bar */
    browser->addr_bar = UIElement_allocate(-1, 20, 1);
    UIElement_set_background(browser->addr_bar, 0, 0xFFFFFF);
    UIElement_set_padding(browser->addr_bar, 2, 2, 2, 6);
    UIElement_set_text(browser->addr_bar, "Address: ", 0x000000, UIE_TEXT_ALIGN_VERT_CENTER);
    UIElement_append_child(browser->app->canvas, browser->addr_bar);
//    browser->app->canvas->background


    browser->app->window->should_render = 1;
    screen_changed = 1;

}
