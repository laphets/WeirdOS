#include "paging.h"

void init_paging()
{
    /**
     * Init page directory for kernel
     */
    page_directory_entry_t *kernel_page_directory = &default_page_directory[1];
    kernel_page_directory->present = 1;
    kernel_page_directory->rw = 1;
    kernel_page_directory->us = 0;
    kernel_page_directory->pwt = 0;
    kernel_page_directory->pcd = 1;
    kernel_page_directory->accessed = 0;
    kernel_page_directory->dirty = 0;
    kernel_page_directory->ps = 1;
    kernel_page_directory->global = 1;
    kernel_page_directory->avail = 0;
    kernel_page_directory->address = (0x1 << 10); /* Set to the 4 MB */

    /**
      * Init page directory and table for video memory
      */
    page_directory_entry_t *first_page_directory = &default_page_directory[0];
    first_page_directory->present = 1;
    first_page_directory->rw = 1;
    first_page_directory->us = 0;
    first_page_directory->pwt = 0;
    first_page_directory->pcd = 0; /* video memory */
    first_page_directory->accessed = 0;
    first_page_directory->dirty = 0;
    first_page_directory->ps = 0;     /* For 4k page */
    first_page_directory->global = 0; /* This bit is ignored for 4K page directory entries */
    first_page_directory->avail = 0;
    first_page_directory->address = ((uint32_t)first_page_table >> 12);

    page_table_entry_t *vm_page_table = &first_page_table[VIDEO_MEMORY_START];
    vm_page_table->present = 1;
    vm_page_table->rw = 1;
    vm_page_table->us = 0; /* may be changed by vidmap(?) */
    vm_page_table->pwt = 0;
    vm_page_table->pcd = 0; /* video memory */
    vm_page_table->accessed = 0;
    vm_page_table->dirty = 0;
    vm_page_table->pat = 0;
    vm_page_table->global = 1; /* set to 1 to enable TLB */
    vm_page_table->avail = 0;
    vm_page_table->address = VIDEO_MEMORY_START; /* Just map to same addr */

    /**
      * Then we enable the paging
      * First set up cr3 to be the page_directory
      * Then set up cr0 to or with 0x80000000 to enable paging
      * Then enable PSE (4 MiB pages) by setting up cr4 bit5 physical address extension
      */
    asm volatile("                  \n\
            movl %0, %%eax           \n\
            movl %%eax, %%cr3        \n\
            movl %%cr4, %%eax        \n\
            orl $0x00000010, %%eax   \n\
            movl %%eax, %%cr4        \n\
            movl %%cr0, %%eax        \n\
            orl $0x80000000, %%eax   \n\
            movl %%eax, %%cr0        \n\
            "
                 :
                 : "r"(default_page_directory)
                 : "eax");
}

/**
 * Set user video memory map to virtual address of start_addr
 * @param start_addr the virtual address to map
 */
void set_user_vm(uint8_t* start_addr) {
    int32_t page_directory_idx = (uint32_t)start_addr / _4MB;
    int32_t page_table_idx = ((uint32_t)start_addr % _4MB) / _4KB;

    page_directory_entry_t *user_vm_page_directory = &default_page_directory[page_directory_idx];
    user_vm_page_directory->present = 1;
    user_vm_page_directory->rw = 1;
    user_vm_page_directory->us = 1;
    user_vm_page_directory->pwt = 0;
    user_vm_page_directory->pcd = 0;
    user_vm_page_directory->accessed = 0;
    user_vm_page_directory->dirty = 0;
    user_vm_page_directory->ps = 0;
    user_vm_page_directory->global = 0;
    user_vm_page_directory->avail = 0;
    user_vm_page_directory->address = ((uint32_t)user_vm_page_table >> 12);

    page_table_entry_t *vm_page_table_entry = &user_vm_page_table[page_table_idx];
    vm_page_table_entry->present = 1;
    vm_page_table_entry->rw = 1;
    vm_page_table_entry->us = 1;
    vm_page_table_entry->pwt = 0;
    vm_page_table_entry->pcd = 0; /* video memory */
    vm_page_table_entry->accessed = 0;
    vm_page_table_entry->dirty = 0;
    vm_page_table_entry->pat = 0;
    vm_page_table_entry->global = 0;
    vm_page_table_entry->avail = 0;
    vm_page_table_entry->address = VIDEO_MEMORY_START; /* Just map to same addr */
}

/**
 * Disable the current mapping from start_addr
 * @param start_addr pointer to current mapped address
 */
void reset_user_vm(uint8_t* start_addr) {
    int32_t page_directory_idx = (uint32_t)start_addr / _4MB;
    int32_t page_table_idx = ((uint32_t)start_addr % _4MB) / _4KB;

    page_directory_entry_t *user_vm_page_directory = &default_page_directory[page_directory_idx];
    user_vm_page_directory->present = 1;
    page_table_entry_t *vm_page_table_entry = &user_vm_page_table[page_table_idx];
    vm_page_table_entry->present = 1;
}

/*
 * https://wiki.osdev.org/TLB
 */
void flush_paging() {
    asm volatile(
            "movl %%cr3, %%eax;"
            "movl %%eax, %%cr3;"
            :
            :
            : "eax"
            );
}
