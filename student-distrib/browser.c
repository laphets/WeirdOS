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

static void dfs(UIElement_t* element, HtmlElement* node) {
    if(node == NULL)
        return;
    for(; node != NULL; node = node->sibling) {
        if(node->text != NULL) {
            /* Then we create a new UI Element */
            UIElement_t* dom_obj = UIElement_allocate(-1, 20, 1);
            UIElement_set_background(dom_obj, 1, 0xFFFFFF);
            UIElement_set_padding(dom_obj, 2, 2, 2, 6);
            UIElement_set_text(dom_obj, node->text, 0x000000, UIE_TEXT_ALIGN_DEFAULT);
            UIElement_append_child(element, dom_obj);

            if(node->tag == HTML_TAG_NONE) {
                kprintf("text: %s", (node->text));
            } else {
                kprintf("element %s: %s", html_tag[node->tag], (node->text));
            }
            dfs(dom_obj, node->child);
        } else {
            kprintf("element: %s ", html_tag[node->tag]);
            dfs(element, node->child);
        }
    }
}

static void parse_http_result(browser_t* browser, http_res_t* response) {
    ((uint8_t*)response->data)[response->length] = '\0';
    uint32_t http_start_ptr = 0;
    int i = 0;
    for(i = 0; i + 1 < response->length; i++) {
        uint8_t byte0 = ((uint8_t*)response->data)[i];
        uint8_t byte1 = ((uint8_t*)response->data)[i+1];
        if(byte0 == '\r' && byte1 == '\n') {
            http_start_ptr = i + 2;
        }
    }
    UIElement_clear_text(browser->content);

    HtmlDocument* doc = html_parse((uint8_t*)((uint32_t)response->data + http_start_ptr), response->length - http_start_ptr);
    if(doc != NULL) {
        dfs(browser->content, doc->root_element);
    } else {
        UIElement_set_text(browser->content, "HTML Parse Error", 0x000000, UIE_TEXT_ALIGN_VERT_CENTER | UIE_TEXT_ALIGN_HORZ_CENTER);
    }
}

static void keyboard_event_handler(browser_t* browser, char ch) {
    if(ch == '\n') {
        char content_text[100];
        sprintf((uint8_t*)content_text, "Processing to %s", browser->addr_input_bar->text);
        UIElement_set_text(browser->content, content_text, 0x000000, UIE_TEXT_ALIGN_VERT_CENTER | UIE_TEXT_ALIGN_HORZ_CENTER);
    } else if (ch == '\b') {
        uint32_t cur_text_len = strlen(browser->addr_input_bar->text);
        if(cur_text_len > 0) {
            browser->addr_input_bar->text[cur_text_len - 1] = '\0';
        }
    } else {
        char* updated_text = kmalloc(strlen(browser->addr_input_bar->text) + 5);
        sprintf((uint8_t*)updated_text, "%s%c", browser->addr_input_bar->text, ch);
        UIElement_set_text(browser->addr_input_bar, updated_text, 0x000000, UIE_TEXT_ALIGN_VERT_CENTER);
        kfree(updated_text);
    }

    browser->app->window->should_render = 1;
    screen_changed = 1;
    render_screen();

    /* Then begin http */
    if(ch == '\n') {
        char domain[100];
        sprintf((uint8_t*)domain, "%s", browser->addr_input_bar->text);
        http_res_t http_response = http_request((uint8_t*)domain);
        parse_http_result(browser, &http_response);
        kfree(http_response.data);

        browser->app->window->should_render = 1;
        screen_changed = 1;
        render_screen();
    }
}

void launch_browser() {
    browser_t* browser = kmalloc(sizeof(browser_t));
    browser->app = new_app("ECE391 Internet Explorer");

    /* Attch browser itself */
    UIWindow_attach_app(browser->app->window, browser);

    /* Init all the handlers */
    browser->app->window->keyboard_event_handler = (keyboard_event_handler_t)keyboard_event_handler;
    browser->app->window->left_click_event_handler = left_click_event_handler;
    browser->app->window->left_release_event_handler = left_release_event_handler;

    /* Draw the layout */
    UIElement_set_background(browser->app->canvas, 0, 0xC4C7CB);

    /* Add address hint bar */
    browser->addr_hint_bar = UIElement_allocate(-1, 20, 1);
    UIElement_set_background(browser->addr_hint_bar, 0, 0xFFFFFF);
    UIElement_set_padding(browser->addr_hint_bar, 2, 2, 2, 6);
    UIElement_set_text(browser->addr_hint_bar, "Type address to below input box", 0x000000, UIE_TEXT_ALIGN_VERT_CENTER);
    UIElement_append_child(browser->app->canvas, browser->addr_hint_bar);

    /* First add the address bar */
    browser->addr_input_bar = UIElement_allocate(-1, 20, 1);
    UIElement_set_margin(browser->addr_input_bar, 4,0,2,0);
    UIElement_set_background(browser->addr_input_bar, 0, 0xFFFFFF);
    UIElement_set_padding(browser->addr_input_bar, 2, 2, 2, 6);
    UIElement_set_text(browser->addr_input_bar, "", 0x000000, UIE_TEXT_ALIGN_VERT_CENTER);
    UIElement_append_child(browser->app->canvas, browser->addr_input_bar);

    /* Then add the content */
    browser->content = UIElement_allocate(-1, -1, 1);
    UIElement_set_margin(browser->content, 6,6,6,6);
    UIElement_set_background( browser->content, 0, 0xFFFFFF);
    UIElement_set_padding( browser->content, 10, 10, 10, 10);
    UIElement_set_text(browser->content, "Welcome to ECE391 Internet Explorer", 0x000000, UIE_TEXT_ALIGN_VERT_CENTER | UIE_TEXT_ALIGN_HORZ_CENTER);
    UIElement_append_child(browser->app->canvas,  browser->content);


    browser->app->window->should_render = 1;
    screen_changed = 1;

}
