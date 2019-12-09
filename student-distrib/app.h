/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/8
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef _APP_H
#define _APP_H

#include "lib.h"
#include "dom.h"

typedef struct app {
    char* name;
    UIWindow_t* window;
    UIElement_t* canvas;

} app_t;

app_t* new_app(char* name);

#endif //MP3_APPLICATION_H
