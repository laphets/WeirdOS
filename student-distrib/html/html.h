/*
 * html - a simple html parser lacking a better name 
 * The contents of this file is licensed under the MIT License,
 * see the file COPYING or http://opensource.org/licenses/MIT
 */

#ifndef __HTML_H_
#define __HTML_H_

#include "stack.h"
#include "attrib.h"
#include "tag.h"
#include "../lib.h"

#ifndef HTML_PARSE_STATE_TYPE
/* Do not expose internals to calling application */
#define HTML_PARSE_STATE_TYPE void
#endif

typedef struct HtmlElement HtmlElement;
struct HtmlElement {
	HtmlTag tag;
	char *tag_name;
	
	char *text;
	HtmlAttrib *attrib;
	HtmlElement *child;
	HtmlElement *sibling;
};

typedef struct HtmlDocument HtmlDocument;
struct HtmlDocument {
	HtmlElement *root_element;
};

typedef HTML_PARSE_STATE_TYPE HtmlParseState;

extern const char const *html_tag[HTML_TAGS];

HtmlTag html_lookup_tag(const char *string);
HtmlParseState *html_parse_begin();
const char *html_parse_stream(HtmlParseState *state, const char *stream, const char *token, size_t len);
HtmlDocument *html_parse_end(HtmlParseState *state);
void *html_free_element(HtmlElement *element);

// printing
void *html_print_dom(HtmlDocument *document);
void *html_print_dom_element(HtmlElement *element, int level);

void *html_free_document(HtmlDocument *document);

#endif
