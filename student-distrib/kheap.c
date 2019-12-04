/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/20
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "kheap.h"

/**
 * We use an O(n) array to maintain the order
 * We can try to improve it by AVL or RBTree in the future
 */
ordered_array_t ordered_array;

void init_ordered_array(uint32_t capacity) {
    ordered_array.size = 0;
    ordered_array.capacity = capacity;
    ordered_array.array = (header_t**)KHEAP_START_ADDR; /* We just set to start addr */
    memset(ordered_array.array, 0, capacity * sizeof(void*));
}

void ordered_array_insert(header_t* elem) {
    int i = 0;
    while(i < ordered_array.size && ordered_array.array[i]->size < elem->size)
        i++;
    if(i == ordered_array.size) {
        ordered_array.array[ordered_array.size++] = elem;
        return;
    }
    /* Then we should insert to the i position */
    header_t* tmp = ordered_array.array[i];
    ordered_array.array[i] = elem;
    while(i < ordered_array.size) {
        i++;
        header_t* next_elem = ordered_array.array[i];
        ordered_array.array[i] = tmp;
        tmp = next_elem;
    }
    ordered_array.size++;
}

header_t* ordered_array_find(uint32_t idx) {
    if(idx >= ordered_array.size)
        return NULL;
    return ordered_array.array[idx];
}


void ordered_array_delete(uint32_t idx) {
    if(idx >= ordered_array.size)
        return;
    while(idx < ordered_array.size - 1) {
        ordered_array.array[idx] = ordered_array.array[idx+1];
        idx++;
    }
    ordered_array.size--;
}

void ordered_array_delete_header(header_t* header) {
    int32_t i = 0;
    for(i = 0;i < ordered_array.size; i++) {
        if(ordered_array.array[i] == header) {
            ordered_array_delete(i);
            break;
        }
    }
}

int32_t find_hole(uint32_t size, uint8_t should_align) {
    uint32_t i = 0;
    while(i < ordered_array.size) {
        header_t* header = ordered_array_find(i);
        if(should_align) {
            /* Then we should calculate the new required hole size for the aligned mapping */
            uint32_t start_addr = (uint32_t)header;
            uint32_t offset = _4KB - (start_addr + sizeof(header_t) % _4KB);
            uint32_t remain_hole_size = header->size - offset;
            if(remain_hole_size >= size)
                break;
        } else if (header->size >= size) {
            break;
        }
        i++;
    }
    if(i == ordered_array.size)
        return -1;
    return i;
}

int32_t heap_expand(uint32_t new_size) {
    if(new_size & 0xFFF) {
        new_size &= 0xFFFFF000;
        new_size += _4KB;
    }
    /* Check whether heap is full */
    if(new_size + heap.start_addr > heap.end_addr)
        return -1;
    uint32_t i = heap.end_addr - heap.start_addr;
    while(i < new_size) {
        alloc_frame(get_page(heap.start_addr+i, 1), 1, 1, 0);
        i += _4KB;
    }
    heap.end_addr = heap.start_addr + new_size;
    return 0;
}

int32_t heap_contract(uint32_t new_size) {
    if(new_size & 0xFFF) {
        new_size &= 0xFFFFF000;
        new_size += _4KB;
    }
    if(new_size < KHEAP_MIN_SIZE)
        return -1;
    uint32_t i = heap.end_addr-heap.start_addr - _4KB;
    while(i > new_size) {
        free_frame(get_page(i, 0));
        i -= _4KB;
    }
    heap.end_addr = heap.start_addr + new_size;
    return new_size;
}

footer_t* getfooter(header_t* header) {
    return (footer_t*)((uint32_t)header + header->size - sizeof(footer_t));
}

uint32_t heap_malloc(uint32_t size, uint8_t should_align) {
    /**
     * For heap malloc, we should do the following things,
     * First we need to find whether there is some hole which can fit ourself
     * If there is no such hole, we should expand the heap +size
     * After we expand some size, we should find the rightest block, if there is no such blcok, we just set for the old right
     * Then we adjust the block, and reinsert into the ordered_array if needed, and recursively call malloc
     * IF there is some good hole, we first check for whether it's seperatable, if not, we update the size
     * Then we check if we should do some page align, if so, we should seperate some new space for the padding, and for that block we do not need to delete
     * IF not, we should delete the origin block
     * Finally, if there we can do seperate, we should
     */
    uint32_t origin_size = size;
    size += sizeof(header_t) + sizeof(footer_t);    /* Update the size for some metadata */
    uint32_t hole_idx = find_hole(size, should_align);
    if(hole_idx  == -1) {
        /* TODO: Check for page aligning here */

        /* So now our heap can't afford the new request size */
        uint32_t old_heap_size = heap.end_addr - heap.start_addr;
        heap_expand(old_heap_size + size);

        /* Thne we try to find the right most block */
        int32_t right_most_idx = -1;
        header_t* right_most_header = NULL;
        int32_t i;
        for(i = 0; i < ordered_array.size; i++) {
            header_t* tmp_header = ordered_array_find(i);
            if(tmp_header > right_most_header) {
                right_most_header = tmp_header;
                right_most_idx = i;
            }
        }

        if(right_most_idx != -1) {
            /* IF there is such block */
            /* Then we should pop it and update it's size and re-insert */
            ordered_array_delete(right_most_idx);
            right_most_header->size += size;
            footer_t* right_most_footer = getfooter(right_most_header);
            right_most_footer->magic = KHEAP_MAGIC;
            right_most_footer->header = right_most_header;
            ordered_array_insert(right_most_header);

        } else {
            /* IF there is no such block, we create a new hole then, and insert into the array */
            right_most_header = (header_t*)(heap.start_addr + old_heap_size);
            right_most_header->magic = KHEAP_MAGIC;
            right_most_header->size = size;
            right_most_header->is_hole = 1;
            footer_t* right_most_footer = getfooter(right_most_header);
            right_most_footer->magic = KHEAP_MAGIC;
            right_most_footer->header = right_most_header;
            ordered_array_insert(right_most_header);
        }

        /* Then we recursively call malloc, maybe it's bad, whatever */
        return heap_malloc(origin_size, should_align);
    }
    /* So we have find the hole and assign that hole to this alloc request */
    header_t* header = ordered_array_find(hole_idx);
    footer_t* footer = getfooter(header);

    /* First check for whether it's sepeartable */
    if((header->size - size) <= (sizeof(header_t) + sizeof(footer_t))) {
        /* If the remain size is bigger than metadata size, we do seperate */
        size = header->size;
    }

    if(should_align && ((uint32_t)header & 0xFFF) != 0) {
        uint32_t offset = _4KB - (((uint32_t)header + sizeof(header_t)) % _4KB);
        /* TODO: Check offset size here */

        /* Then from head to offset is the remaining offseted block */
        ordered_array_delete(hole_idx);
        header_t* offseted_header = header;
        offseted_header->size = offset;
        offseted_header->magic = KHEAP_MAGIC;
        offseted_header->is_hole = 1;
        footer_t* offseted_footer = getfooter(offseted_header);
        offseted_footer->magic = KHEAP_MAGIC;
        offseted_footer->header = offseted_header;
        ordered_array_insert(offseted_header);

        header = (header_t*)((uint32_t)offseted_footer + sizeof(footer_t));
        header->magic = KHEAP_MAGIC;
        header->is_hole = 1;
        header->size = (uint32_t)footer - (uint32_t)header;
        footer->header = header;
    } else {
        ordered_array_delete(hole_idx);
    }

    header->is_hole = 0;

    if((header->size - size) > (sizeof(header_t) + sizeof(footer_t))) {
        /* Then we return the first block, and give back the second one */
        header_t* first_header = header;
        first_header->size = size;
        footer_t* first_footer = getfooter(first_header);
        first_footer->header = first_header;
        first_footer->magic = KHEAP_MAGIC;

        header_t* second_header = (header_t*)((uint32_t)first_footer + sizeof(footer_t));
        footer_t* second_footer = footer;

        second_header->size = (uint32_t)second_footer + sizeof(footer_t) - (uint32_t)second_header;
        second_header->is_hole = 1;
        second_header->magic = KHEAP_MAGIC;
        second_footer->magic = KHEAP_MAGIC;
        second_footer->header = second_header;

        ordered_array_insert(second_header);
    }

    return (uint32_t)header + sizeof(header_t);
}

void heap_free(void* target) {
    /**
     * For heap free, we should do the following things,
     * Get the target itself, and prepare to mark as hole
     * Check for left block, whether it's a hole, if so, merge (in this case we do not need to readd the new hole)
     * Check for the right block, if hole, we can merge that block
     * Then check for whether we can contract the heap
     * Finally we can insert the hole if needed
     */

    /* Check for input */
    if(target == NULL)
        return;

    header_t* target_header = target - sizeof(header_t);
    ASSERT(target_header->magic == KHEAP_MAGIC);
    if(target_header->magic != KHEAP_MAGIC)
        return;
    footer_t* target_footer = getfooter(target_header);
    ASSERT(target_footer->magic == KHEAP_MAGIC);
    if(target_footer->magic != KHEAP_MAGIC)
        return;

    ordered_array_delete_header(target_header);

    /* Then check for the left blcok */
    footer_t* left_footer = (target - sizeof(footer_t));
    if((uint32_t)left_footer > heap.start_addr && left_footer->magic == KHEAP_MAGIC) {
        header_t* left_header = left_footer->header;
        if(left_header->magic == KHEAP_MAGIC && left_header->is_hole) {
            /* Then we merge those holes */
            ordered_array_delete_header(left_header);
            target_header = left_header;
            target_header->is_hole = 1;
            target_footer->header = target_header;
            target_header->size = (uint32_t)target_footer - (uint32_t)target_header + sizeof(footer_t);
        }
    }

    /* Then check for the right block */
    header_t* right_header = (header_t*)((uint32_t)target_footer + sizeof(footer_t));
    if((uint32_t)right_header < heap.end_addr && right_header->magic == KHEAP_MAGIC) {
        footer_t* right_footer = getfooter(right_header);
        ASSERT(right_footer->magic == KHEAP_MAGIC);
        if(right_footer->magic == KHEAP_MAGIC) {
            /* Then we merge holes */
            ordered_array_delete_header(right_header);
            target_footer = right_footer;
            target_header->size = (uint32_t)target_footer - (uint32_t)target_header + sizeof(footer_t);
        }
    }

    /* TODO: Then we check whether we can contrat the heap */

    /* Finally, we readd the hole */
    target_header->is_hole = 1;
    ordered_array_insert(target_header);
}

uint32_t vitrual2phys(uint32_t vitual_addr) {
    if(heap.present == 1) {
        page_table_entry_t* pte = get_page(vitual_addr, 0);
        return (pte->address << 12) + (vitual_addr&0xFFF);
    }

    return 0;
}

void init_heap() {
    kprintf("Begin init heap...\n");
    init_ordered_array(KHEAP_ORDERED_ARRAY_SIZE);
    heap.start_addr = KHEAP_START_ADDR + KHEAP_ORDERED_ARRAY_SIZE;
    heap.end_addr = KHEAP_START_ADDR + KHEAP_INIT_SIZE;
    heap.max_addr = -1; /* TODO: not available now */

    if(heap.start_addr & 0xFFF) {
        heap.start_addr &= 0xFFFFF000;
        heap.start_addr += _4KB;
    }

    header_t* header = (header_t*)heap.start_addr;
    header->magic = KHEAP_MAGIC;
    header->is_hole = 1;
    header->size = heap.end_addr - heap.start_addr;

    footer_t* footer = getfooter(header);
    footer->magic = KHEAP_MAGIC;
    footer->header = header;

    ordered_array_insert(header);

    heap.present = 1;
}
