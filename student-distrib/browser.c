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


void removeChar(char *s, int c){

    int j, n = strlen(s);
    int i;
    for ( i=j=0; i<n; i++)
        if (s[i] != c)
            s[j++] = s[i];

    s[j] = '\0';
}

char* trim(char *str)
{
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && (isspace((unsigned char)*end) || ((unsigned char)*end) == '\n')) end--;

    // Write new null terminator character
    end[1] = '\0';

    removeChar(str, '\n');
    return str;
}

char* find_text_of_child(HtmlElement* element) {
    while(element != NULL) {
        if(element->text != NULL && strlen(element->text) != 0) {
            return element->text;
        }
        char* child = find_text_of_child(element->child);
        if(child != NULL)
            return child;
        element = element->sibling;
    }
    return NULL;
}

inline void append_new_any(char* result, char append_char) {
    uint32_t len = strlen(result);
    result[len] = append_char;
    result[len+1] = '\0';
}

void dfs(HtmlElement* element, int level, char* result) {
    while(element != NULL) {
        int nextlevel = level;
        if(element->text != NULL) {
            nextlevel++;
            int i;
            for(i = 0; i < level; i++) {
                strcat(result, " ");
//                sprintf(result, "%s ", result);
//                printf("-");
            }

            /* This is a text component */
//            printf("%s", trim(element->text));
            strcat(result, trim(element->text));
//            sprintf(result, "%s%s", result,  trim(element->text));
        } else {
//            if(element->tag_name != NULL) {
//                kprintf("element.tag: %d, %s\n", element->tag, element->tag_name);
//            }
            if(element->tag == HTML_TAG_P || element->tag == HTML_TAG_H1 || element->tag == HTML_TAG_H2 || element->tag == HTML_TAG_H3) {
//                strcat(result, "\n");
//                dfs(element->child, nextlevel, result);
//                element = element->sibling;
//                strcat(result, "\n");
//                continue;
            }
            if(element->tag == HTML_TAG_BR) {
                strcat(result, "\n");
                kprintf("\nadd newline\n");
//                sprintf(result, "%s\n", result);
//                printf(("||\n"));
                element = element->sibling;
                continue;
            }
            if(element->tag == HTML_TAG_TITLE) {
                kprintf("We find the title: %s!!!\n", trim(element->child->text));
                element = element->sibling;
                continue;
            }
            if(element->tag == HTML_TAG_UL) {
                nextlevel++;
                strcat(result, "\n");
                kprintf("\nadd newline\n");

//                printf("||\n");
            }
            if(element->tag == HTML_TAG_LI) {
//                strcat(result, "\n");
                kprintf("\nadd newline\n");

//                printf("||\n");
            }

            if(element->tag == HTML_TAG_A) {
                HtmlAttrib *attrib = element->attrib;
                char* href = NULL;
                if (attrib) {
                    do{
                        if (attrib->key == HTML_ATTRIB_HREF) {
                            href = attrib->value;
                        } else {

                        }
                        attrib = attrib->next;
                    }while(attrib);
                }
                if(href != NULL) {
                    char* a_text = find_text_of_child(element->child);
                    if(a_text != NULL) {
                        strcat(result, " ");
                        strcat(result, trim(find_text_of_child(element->child)));
                        strcat(result, " ");
//                        sprintf(result, "%s[%s](%s)", result, trim(find_text_of_child(element->child)), href);
//                        printf("[%s](%s)", trim(find_text_of_child(element->child)), href);
                        element = element->sibling;
                        continue;
                    }
                }
            }

        }
        dfs(element->child, nextlevel, result);
        element = element->sibling;
    }
}

//static void dfs(UIElement_t* element, HtmlElement* node) {
//    if(node == NULL)
//        return;
//    for(; node != NULL; node = node->sibling) {
//        if(node->text != NULL) {
//            /* Then we create a new UI Element */
//            UIElement_t* dom_obj = UIElement_allocate(-1, 20, 1, 0);
//            UIElement_set_background(dom_obj, 1, 0xFFFFFF);
//            UIElement_set_padding(dom_obj, 2, 2, 2, 6);
//            UIElement_set_text(dom_obj, node->text, 0x000000, UIE_TEXT_ALIGN_DEFAULT);
//            UIElement_append_child(element, dom_obj);
//
//            if(node->tag == HTML_TAG_NONE) {
//                kprintf("text: %s", (node->text));
//            } else {
//                kprintf("element %s: %s", html_tag[node->tag], (node->text));
//            }
//            dfs(dom_obj, node->child);
//        } else {
//            kprintf("element: %s ", html_tag[node->tag]);
//            dfs(element, node->child);
//        }
//    }
//}

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
        char* result = kmalloc(response->length);
        memset(result, 0, response->length);
        dfs(doc->root_element, 0, result);
        UIElement_set_text(browser->content, result, 0x000000, UIE_TEXT_ALIGN_DEFAULT);
//        gui_debug(result);

//        dfs(browser->content, doc->root_element);
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
    browser->addr_hint_bar = UIElement_allocate(-1, 20, 1, 0);
    UIElement_set_background(browser->addr_hint_bar, 0, 0xFFFFFF);
    UIElement_set_padding(browser->addr_hint_bar, 2, 2, 2, 6);
    UIElement_set_text(browser->addr_hint_bar, "Type address to below input box", 0x000000, UIE_TEXT_ALIGN_VERT_CENTER);
    UIElement_append_child(browser->app->canvas, browser->addr_hint_bar);

    /* First add the address bar */
    browser->addr_input_bar = UIElement_allocate(-1, 20, 1, 0);
    UIElement_set_margin(browser->addr_input_bar, 4,0,2,0);
    UIElement_set_background(browser->addr_input_bar, 0, 0xFFFFFF);
    UIElement_set_padding(browser->addr_input_bar, 2, 2, 2, 6);
    UIElement_set_text(browser->addr_input_bar, "", 0x000000, UIE_TEXT_ALIGN_VERT_CENTER);
    UIElement_append_child(browser->app->canvas, browser->addr_input_bar);

    /* Then add the content */
    browser->content = UIElement_allocate(-1, -1, 1, 0);
    UIElement_set_margin(browser->content, 6,6,6,6);
    UIElement_set_background( browser->content, 0, 0xFFFFFF);
    UIElement_set_padding( browser->content, 10, 10, 10, 10);
    UIElement_set_text(browser->content, "Welcome to ECE391 Internet Explorer", 0x000000, UIE_TEXT_ALIGN_VERT_CENTER | UIE_TEXT_ALIGN_HORZ_CENTER);
    UIElement_append_child(browser->app->canvas,  browser->content);

    gui_debug("Browser has started");


    browser->app->window->should_render = 1;
    screen_changed = 1;

}
