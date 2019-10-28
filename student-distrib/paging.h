#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"
#include "lib.h"

#define PAGE_DIRECTORY_SIZE 1024
#define PAGE_TABLE_SIZE 1024
#define PAGE_ALIGN_SIZE 4096
#define VIDEO_MEMORY_START 0xB8
#define VIDEO_MEMORY_START_ADDRESS 0xB8000

typedef struct page_table_entry {
    uint32_t present : 1;    /* 1->valid, 0->not used */
    uint32_t rw         : 1; /* 1->readwrite, 0->only read */
    uint32_t us       : 1;   /* 1->user level, 0->supervisor-only */
    uint32_t pwt      : 1; /* should always be 0 */
    uint32_t pcd      : 1; /* 1->code and data, 0->video memory */
    uint32_t accessed   : 1;   /* just set to 0 */
    uint32_t dirty      : 1;   /* Init with 0 */
    uint32_t pat      :1; /* Set to 0 */
    uint32_t global :1; /* global bit, set to 1 for kernel */
    uint32_t avail :3; /* Set to 0 */
    uint32_t address : 20; /* the base address for page frame */
} page_table_entry_t;

typedef struct page_directory_entry {
    uint32_t present : 1;    /* 1->valid, 0->not used */
    uint32_t rw         : 1; /* 1->readwrite, 0->only read */
    uint32_t us       : 1;   /* 1->user level, 0->supervisor-only */
    uint32_t pwt      : 1; /* should always be 0 */
    uint32_t pcd      : 1; /* 1->code and data, 0->video memory */
    uint32_t accessed   : 1;   /* just set to 0 */
    uint32_t dirty      : 1;   /* Init with 0 */
    uint32_t ps      :1; /* Page size, 1 for 4M page directory(kernel), 0 for 4K page directory(video memory) */
    uint32_t global :1; /* global bit, set to 1 for kernel */
    uint32_t avail :3; /* Set to 0 */
    uint32_t address : 20; /* the base address for page frame */
} page_directory_entry_t;

page_directory_entry_t default_page_directory[PAGE_DIRECTORY_SIZE] __attribute__((aligned(PAGE_ALIGN_SIZE)));
page_table_entry_t first_page_table[PAGE_TABLE_SIZE] __attribute__((aligned(PAGE_ALIGN_SIZE)));

void init_paging();

#endif
