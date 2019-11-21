/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/20
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "kheap.h"

kheap_t heap;
ordered_array_t ordered_array;

void init_ordered_array(uint32_t capacity) {
    ordered_array.size = 0;
    ordered_array.capacity = capacity;
    ordered_array.array = kmalloc(capacity * sizeof(void*), 0);
    memset(ordered_array.array, 0, capacity * sizeof(void*));
}

void init_heap() {

}


void* malloc(uint32_t size, uint8_t should_align) {

}

void free(void* data) {

}