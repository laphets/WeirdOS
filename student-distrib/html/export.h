/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/8
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#ifndef MP3_EXPORT_H
#define MP3_EXPORT_H

#include "html.h"
#include "util.h"


inline uint8_t is_tag(char* given, char* target);
HtmlDocument* html_parse(uint8_t* buffer, uint32_t length);

#endif //MP3_EXPORT_H
