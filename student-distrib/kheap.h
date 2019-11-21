/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/20
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 * Ref: http://www.jamesmolloy.co.uk/tutorial_html/7.-The%20Heap.html
 */
#ifndef _KHEAP_H
#define _KHEAP_H

#include "lib.h"

#define KHEAP_MAGIC 0xDEADBEAF  /* Really a magic number */

typedef struct {
    uint32_t magic;
    uint8_t is_hole;
    uint32_t size; /* Include head and footer */
} header_t;

typedef struct {
    uint32_t magic;
    header_t *header;   /* Pointer to the header */
} footer_t;

typedef struct {
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t max_addr;
} kheap_t;

typedef struct {
    void* array;
    uint32_t size;
    uint32_t capacity;
} ordered_array_t;

#endif //MP3_KHEAP_H
