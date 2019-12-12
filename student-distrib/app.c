/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/8
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "app.h"

app_t* new_app(char* name) {
    app_t* app = kmalloc(sizeof(app_t));

    /* Init the texts */
    uint32_t length = strlen(name);
    app->name = kmalloc(length + 1);
    memcpy(app->name, name, length);
    app->name[length] = '\0';

    /* Init the window */
    app->window = UIWindow_allocate(100, 100, 800, 600, 1);
    app->canvas = gui_render_application_window(app->window, app->name);
    UIWindow_append(app->window);

    screen_changed = 1;

    return app;
}
