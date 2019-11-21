#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"
#include "lib.h"

#define PAGE_DIRECTORY_SIZE 1024
#define PAGE_TABLE_SIZE 1024
#define PAGE_ALIGN_SIZE 4096
#define VIDEO_MEMORY_START 0xB8
#define VIDEO_MEMORY_START_ADDRESS 0xB8000

#define _4MB 0x400000
#define _4KB 0x1000

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

//typedef struct page_directory {
//    page_directory_entry_t entry_list[PAGE_DIRECTORY_SIZE] __attribute__((aligned(PAGE_ALIGN_SIZE)));
//    page_table_entry_t* default_page_table_list[]
//} page_directory_t;

page_directory_entry_t default_page_directory[PAGE_DIRECTORY_SIZE] __attribute__((aligned(PAGE_ALIGN_SIZE)));
page_table_entry_t first_page_table[PAGE_TABLE_SIZE] __attribute__((aligned(PAGE_ALIGN_SIZE)));

/* The page table is used to mapping for user space video memory */
page_table_entry_t user_vm_page_table[PAGE_TABLE_SIZE] __attribute__((aligned(PAGE_ALIGN_SIZE)));

/**
 * Init for kernel and video memory paging structure
 */
void init_paging(uint32_t mem_upper);

/*
 * Flush paging by reset cr3
 * https://wiki.osdev.org/TLB
 */
void flush_paging();

/**
 * Set video memory paging into somewhere under kernel space
 * This function is mostly used by setting to an unshown addr for inactive terminal
 * @param phys_addr the phys_addr we want to set into
 */
void set_kernel_vm(uint8_t* phys_addr);

/**
 * Set user video memory map to virtual address of start_addr
 * @param start_addr the virtual address to map
 */
void set_user_vm(char* video_mem, uint8_t* start_addr);

/**
 * Disable the current mapping from start_addr
 * @param start_addr pointer to current mapped address
 */
void reset_user_vm(uint8_t* start_addr);

uint32_t placement_addr;
#define MAX_FRAMES_NUM 64000
#define START_PLACEMENT_ADDR 0xA000000 /* We start from 160MB */
uint32_t frames_num;
/* Frames store the physical avaliablity */
uint32_t* frames; /* Each element can store the status of 32 pdes */

uint32_t kmalloc(uint32_t size, uint8_t should_align);
page_table_entry_t* get_page(uint32_t addr, int8_t shoud_make);
void alloc_frame(page_table_entry_t* pte, int is_kernel, int rw, int for_video);

#endif
