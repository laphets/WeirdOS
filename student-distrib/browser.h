/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/8
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _BROWSER_H
#define _BROWSER_H

#include "lib.h"
#include "app.h"
#include "http.h"

typedef struct browser {
    app_t* app;

    UIElement_t* addr_input_bar;
    UIElement_t* addr_hint_bar;
    UIElement_t* content;

} browser_t;

void launch_browser();

#endif //MP3_BROWSER_H
