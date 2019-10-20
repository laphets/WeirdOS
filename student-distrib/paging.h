#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

#define PAGE_DIRECTORY_SIZE 1024
#define PAGE_TABLE_SIZE 1024
#define PAGE_ALIGN_SIZE 4096

typedef struct page_table_entry {
    uint8_t present : 1;    /* 1->valid, 0->not used */
    uint8_t rw         : 1; /* 1->readwrite, 0->only read */
    uint8_t us       : 1;   /* 1->user level, 0->supervisor-only */
    uint8_t pwt      : 1; /* should always be 0 */
    uint8_t pcd      : 1; /* 1->code and data, 0->video memory */
    uint8_t accessed   : 1;   /* just set to 0 */
    uint8_t dirty      : 1;   /* Init with 0 */
    uint8_t pat      :1; /* Set to 0 */
    uint8_t global :1; /* global bit, set to 1 for kernel */
    uint8_t avail :3; /* Set to 0 */
    uint32_t address : 20; /* the base address for page frame */
} page_table_entry_t;

typedef struct page_directory_entry {
    uint8_t present : 1;    /* 1->valid, 0->not used */
    uint8_t rw         : 1; /* 1->readwrite, 0->only read */
    uint8_t us       : 1;   /* 1->user level, 0->supervisor-only */
    uint8_t pwt      : 1; /* should always be 0 */
    uint8_t pcd      : 1; /* 1->code and data, 0->video memory */
    uint8_t accessed   : 1;   /* just set to 0 */
    uint8_t dirty      : 1;   /* Init with 0 */
    uint8_t ps      :1; /* Page size, 1 for 4M page directory(kernel), 0 for 4K page directory(video memory) */
    uint8_t global :1; /* global bit, set to 1 for kernel */
    uint8_t avail :3; /* Set to 0 */
    uint32_t address : 20; /* the base address for page frame */
} page_directory_entry_t;


#endif
