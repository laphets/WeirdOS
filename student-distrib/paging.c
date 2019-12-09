#include "paging.h"


/* Then the following code will provide some memory management */
/* Ref: http://www.jamesmolloy.co.uk/tutorial_html/6.-Paging.html */

/**
 * Init for the frame status
 * @param mem_upper
 */
void init_frame_status(uint32_t mem_upper) {
    placement_addr = START_PLACEMENT_ADDR;
    frames_num = (mem_upper - placement_addr) / _4KB;
    frames = kmalloc_a(sizeof(uint32_t) * (frames_num/32), 0);
    memset_dword(frames, 0, frames_num/32);
//    placement_addr = START_PLACEMENT_ADDR;
//    frames_num = (mem_upper - placement_addr) / _4KB;
//    if(frames_num > MAX_FRAMES_NUM)
//        frames_num = MAX_FRAMES_NUM;
}

/**
 * Init for kernel and video memory paging structure
 */
void init_paging(uint32_t mem_upper)
{
    heap.present = 0;
    init_frame_status(mem_upper);
    memset(default_page_table_virtual_addr, 0 ,4 * PAGE_DIRECTORY_SIZE);
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


    uint32_t vm_entry = 0xA0;
    for(vm_entry = 0xA0; vm_entry <= 0xBF; vm_entry++) {
        set_kernel_vm((uint8_t*)(vm_entry << 12));
    }

    /**
     * Set for SVGA mapping
     */
    uint32_t vbe_idx = (VBE_ADDR >> 22);
    default_page_directory[vbe_idx].present = 1;
    default_page_directory[vbe_idx].us = 0;
    default_page_directory[vbe_idx].rw = 1;
    default_page_directory[vbe_idx].global = 1;
    default_page_directory[vbe_idx].ps = 1;
    default_page_directory[vbe_idx].address = (vbe_idx << 10);

//    default_page_directory[vbe_idx].address =
    default_page_directory[vbe_idx+1].present = 1;
    default_page_directory[vbe_idx+1].us = 0;
    default_page_directory[vbe_idx+1].rw = 1;
    default_page_directory[vbe_idx+1].global = 1;
    default_page_directory[vbe_idx+1].ps = 1;
    default_page_directory[vbe_idx+1].address = ((vbe_idx+1) << 10);

    /* Then we should pre map some paging for our heap */
    int32_t i;
    for(i = KHEAP_START_ADDR; i < KHEAP_START_ADDR+KHEAP_INIT_SIZE; i+= _4KB) {
        get_page(i, 1);
    }
//    for(i = KHEAP_START_ADDR; i < KHEAP_START_ADDR+KHEAP_INIT_SIZE; i+= _4KB) {
//        alloc_frame(get_page(i, 1), 1, 1, 0);
//    }

    /* Allocate dynamic parts, and init for their frame */
    i = START_PLACEMENT_ADDR;
    while(i < placement_addr) {
        kprintf("i: 0x%x placement_addr: 0x%x\n", i, placement_addr);
        alloc_frame(get_page(i, 1), 1, 1, 0);
        i += _4KB;
    }

    /* Then we should pre map some paging for our heap */
    for(i = KHEAP_START_ADDR; i < KHEAP_START_ADDR+KHEAP_INIT_SIZE; i+= _4KB) {
        alloc_frame(get_page(i, 1), 1, 1, 0);
    }

    kprintf("Begin freshing paging...\n");

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

    /* Finally we our heap in paging setuped mode */
    init_heap();
}

/**
 * Set video memory paging into somewhere under kernel space
 * This function is mostly used by setting to an unshown addr for inactive terminal
 * @param phys_addr the phys_addr we want to set into
 */
void set_kernel_vm(uint8_t* phys_addr) {
    page_table_entry_t *vm_page_table = &first_page_table[(uint32_t)phys_addr >> 12];
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
    vm_page_table->address = ((uint32_t)phys_addr >> 12); /* Just map to same addr */
}

/**
 * Set user video memory map to virtual address of start_addr
 * @param start_addr the virtual address to map
 */
void set_user_vm(char* video_mem, uint8_t* start_addr) {
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
    vm_page_table_entry->address = ((uint32_t)video_mem >> 12); /* Just map to same addr */
}

/**
 * Disable the current mapping from start_addr
 * @param start_addr pointer to current mapped address
 */
void reset_user_vm(uint8_t* start_addr) {
    int32_t page_directory_idx = (uint32_t)start_addr / _4MB;
    int32_t page_table_idx = ((uint32_t)start_addr % _4MB) / _4KB;

    page_directory_entry_t *user_vm_page_directory = &default_page_directory[page_directory_idx];
    user_vm_page_directory->present = 0;
    page_table_entry_t *vm_page_table_entry = &user_vm_page_table[page_table_idx];
    vm_page_table_entry->present = 0;
}

/*
 * Flush paging by reset cr3
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

/**
 * Allocate some memory
 * @param size
 * @param should_align
 * @return
 */
void* kmalloc_a(uint32_t size, uint8_t should_align) {
    if(heap.present == 1) {
        /**
         * When heap is inited, we will just use routine from heap
         */
        return (void*)heap_malloc(size, should_align);
    } else {
        /**
         * When heap is not inited, we will use the following subroutine
         */
        if(should_align && (placement_addr & 0xFFF)) {
            placement_addr &= 0xFFFFF000;
            placement_addr += _4KB;
        }
        uint32_t begin = placement_addr;
        placement_addr += size;
        return (void*)begin;
    }
}

void* kmalloc(uint32_t size) {
//    kprintf("---MALLOC NEW SPACE----\n");
    return kmalloc_a(size, 0);
}

void kfree(void* target) {
    heap_free(target);
}

void set_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr/_4KB;
    uint32_t index = frame / 32;
    uint32_t offset = frame % 32;
    frames[index] |= (1 << offset);
}
void clear_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr/_4KB;
    uint32_t index = frame / 32;
    uint32_t offset = frame % 32;
    frames[index] &= ~(1 << offset);
}
uint32_t test_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr/_4KB;
    uint32_t index = frame / 32;
    uint32_t offset = frame % 32;
    return (frames[index] & (1 << offset));
}
uint32_t get_avail_frame() {
    uint32_t i, j;
    for(i = 0; i < frames_num / 32; i++) {
        if(frames[i] == 0xFFFFFFFF) {
            /* This 32 frames are all full, we go to next one */
            continue;
        }
        for(j = 0; j < 32; j++) {
            if((frames[i] & (1 << j)) == 0) {
                return i * 32 + j;
            }
        }
    }
    /* We just assume there will always be available frames for us to use */
    return 0;
}

/* We will just use simple bitmap instead of buddy system */
/**
 * The following function will bind the page to an available frame
 * @param pte
 * @param is_kernel
 * @param rw
 * @param for_video
 */
void alloc_frame(page_table_entry_t* pte, int is_kernel, int rw, int for_video) {
    if(pte->address != 0)
        return;
    uint32_t frame_idx = get_avail_frame();
    set_frame(frame_idx * _4KB);
    pte->present = 1;
    pte->rw = rw;
    pte->us = (is_kernel)? 0 : 1;
    pte->global = (is_kernel)? 1 : 0;
    pte->pwt = 0;
    pte->pcd = (for_video)? 0 : 1;
    pte->accessed = 0;
    pte->dirty = 0;
    pte->pat = 1;
    pte->avail = 0;
    pte->address = (START_PLACEMENT_ADDR >> 12) + frame_idx;
}
/**
 * Free frame of some page table
 * @param pte
 */
void free_frame(page_table_entry_t* pte) {
    if(pte->address == 0)
        return;
    clear_frame(pte->address << 12);
    pte->address = 0;
    pte->present = 0;
}

/**
 * Get page table by some index
 * @param pd_index
 * @return
 */
page_table_entry_t* get_pagetable(uint32_t pd_index) {
    if(pd_index < 0 || pd_index >= PAGE_DIRECTORY_SIZE)
        return NULL;
    page_directory_entry_t* pde = &default_page_directory[pd_index];
//    if(pde->present == 0 || pde->ps != 0)
//        return NULL;
    if(heap.present != 1) {
        return (page_table_entry_t*)(pde->address << 12);
    }

    return (page_table_entry_t*)default_page_table_virtual_addr[pd_index];
}

/**
 * Get a page for a address, should_make will make one if not exist
 * @param addr
 * @param shoud_make
 * @return
 */
page_table_entry_t* get_page(uint32_t addr, int8_t shoud_make) {
    addr /= _4KB;
    uint32_t pd_idx = addr / 1024; /* Get the first 10 bits */
    uint32_t pt_idx = addr % 1024;
    page_directory_entry_t* pde = &default_page_directory[pd_idx];

    if(pde->present == 1 && get_pagetable(pd_idx) != 0 && get_pagetable(pd_idx)[pt_idx].present == 1) {
        return &(get_pagetable(pd_idx)[pt_idx]);
    } else if (shoud_make == 1) {
        if(pde->present == 0) {
            page_table_entry_t* page_table = kmalloc_a(PAGE_TABLE_SIZE * sizeof(page_table_entry_t), 1);
            memset(page_table, 0, PAGE_TABLE_SIZE * sizeof(page_table_entry_t));
//            int32_t i = 0;
//            for(i = 0; i < PAGE_TABLE_SIZE; i++) {
//                page_table[i].present = 0;
//                page_table[i].address = 0;
//            }
            default_page_table_virtual_addr[pd_idx] = (uint32_t)page_table;
            /* We should set to the phys addr of allocated page_table */
            uint32_t phys_page_table = vitrual2phys((uint32_t)page_table);
            pde->address = ((uint32_t)phys_page_table >> 12);
            pde->present = 1;
        }
        page_table_entry_t* page_table = get_pagetable(pd_idx);
        page_table_entry_t* pte = &page_table[pt_idx];
        pte->present = 0;
        pte->address = NULL;
        return pte;
    } else {
        return NULL;
    }
}
