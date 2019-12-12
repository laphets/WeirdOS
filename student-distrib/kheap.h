/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/20
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 * Ref: http://www.jamesmolloy.co.uk/tutorial_html/7.-The%20Heap.html
 */
#ifndef _KHEAP_H
#define _KHEAP_H

#include "lib.h"
#include "debug.h"

#define KHEAP_MAGIC 0xDEADBEAF  /* Really a magic number */
#define KHEAP_MIN_SIZE 0x1000000
#define KHEAP_INIT_SIZE 0x1000000 /* Set to 8 MB */
#define KHEAP_ORDERED_ARRAY_SIZE 0x40000
#define KHEAP_START_ADDR 0xB000000

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
    uint8_t present;
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t max_addr;
} kheap_t;

typedef struct {
    header_t** array;
    uint32_t size;
    uint32_t capacity;
} ordered_array_t;

kheap_t heap;

uint32_t vitrual2phys(uint32_t vitual_addr);

uint32_t heap_malloc(uint32_t size, uint8_t should_align);
void heap_free(void* target);
void init_heap();

#endif //MP3_KHEAP_H
