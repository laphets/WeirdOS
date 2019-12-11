/*
 * html - a simple html parser lacking a better name 
 * The contents of this file is licensed under the MIT License,
 * see the file COPYING or http://opensource.org/licenses/MIT
 */

#include "export.h"

inline uint8_t is_tag(char* given, char* target) {
    uint32_t length = strlen(given), i;
    if(length != strlen(target))
        return 0;
    for(i = 0; i < length; i++) {
        if(tolower(given[i]) != target[i]) {
            return 0;
        }
    }
    return 1;
}

void recursive_op(HtmlElement* node) {
    if(node == NULL)
        return;
    for(; node != NULL; node = node->sibling) {
        if(node->text != NULL) {
            if(node->tag == HTML_TAG_NONE) {
                kprintf("text: %s", (node->text));
            } else {
                kprintf("element %s: %s", html_tag[node->tag], (node->text));
            }

        } else {
            kprintf("element: %s ", html_tag[node->tag]);
        }

        recursive_op(node->child);
    }
}

HtmlDocument* html_parse(uint8_t* buffer, uint32_t length) {
    HtmlDocument *doc;
    HtmlParseState *parse_state;

    parse_state = html_parse_begin();
    html_parse_stream(parse_state, (const char*)buffer, (const char*)buffer, length);

    doc = html_parse_end(parse_state);
//    recursive_op(doc->root_element);

    return doc;

//    html_print_dom(doc);
//
//    html_free_document(doc);
}

