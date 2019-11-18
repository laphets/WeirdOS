/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/11/13
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "scheduler.h"

/**
 * Init for scheduler that nothing is currently running
 */
void init_scheduler() {
    current_running_terminal = -1;
}

/**
 * The following code is Deprecated!!
 * The feature is moved into terminal.c instead
 */
void schedule() {
    if(current_running_terminal == -1 || current_active_terminal == -1)
        return;
    int32_t turns = 0;
    next_running_terminal = current_running_terminal;

    for(turns = 0; turns <= MAX_TERMINAL_NUM; turns++) {
        next_running_terminal = (next_running_terminal+1) % MAX_TERMINAL_NUM;
        if(terminal_list[next_running_terminal].present != 1)
            continue;
        if(next_running_terminal == current_running_terminal)
            continue;

        kprintf("Begin schedule...\n");
        /* Then we find the next terminal to run, context switch to there */
        task_t* current_task = get_task_by_pid(terminal_list[current_running_terminal].current_task_pid);
        task_t* next_task = get_task_by_pid(terminal_list[next_running_terminal].current_task_pid);

        /*
        terminal_t* current_terminal = &terminal_list[current_running_terminal];
        terminal_t* next_terminal = &terminal_list[next_running_terminal];
         */

        /**
         * TODO: below
         * If next_task is in inactive terminal, we should set the video paging mapping to some unshown phys address
         * If next_task is in active terminal, we just set back video paging mapping to real address
         */
//        if(next_running_terminal == current_active_terminal) {
//            /* If next_task is in active terminal, we just set back video paging mapping to real address */
//            page_table_entry_t *vm_page_table = &first_page_table[VIDEO_MEMORY_START];
//            vm_page_table->address = (VIDEO_MEMORY_START_ADDRESS >> 12);
////            flush_paging();
////            memcpy(VIDEO_MEMORY_START_ADDRESS, next_terminal->unshown_vm_addr, _4KB);
//        } else {
//            /* If next_task is in inactive terminal, we should set the video paging mapping to some unshown phys address */
//            page_table_entry_t *vm_page_table = &first_page_table[VIDEO_MEMORY_START];
//            vm_page_table->address = (next_terminal->unshown_vm_addr >> 12);
////            flush_paging();
//        }


//        /* We should also update for the cursor */
//        current_terminal->screen_x = get_screen_x();
//        current_terminal->screen_y = get_screen_y();
//        update_cursor_pos(next_terminal->screen_x, next_terminal->screen_y);

        //printf("current_running_terminal: %d, next_running_terminal: %d\n", current_running_terminal, next_running_terminal);

        current_running_terminal = next_running_terminal;
        /* We should re-map the user paging */
        /* Reset parent's page */
        page_directory_entry_t *user_page_directory = &default_page_directory[32];
        user_page_directory->present = 1;
        user_page_directory->rw = 1;
        user_page_directory->us = 1;
        user_page_directory->pwt = 0;
        user_page_directory->pcd = 1;
        user_page_directory->accessed = 0;
        user_page_directory->dirty = 0;
        user_page_directory->ps = 1;
        user_page_directory->global = 0;
        user_page_directory->avail = 0;
        user_page_directory->address = ((2+next_task->pid) << 10);

        flush_paging();

        /* We should modify the TSS */
        tss.ss0 = KERNEL_DS;
        tss.esp0 = KERNEL_BOTTOM - next_task->pid * PCB_BLOCK_SIZE ;

        kprintf("Begin exchange stack[0/2]\n");
        /* Then do some context switch */
        /* Record currrent esp and ebp state */
        asm volatile("                  \n\
            movl %%ebp, %0        \n\
            movl %%esp, %1           \n\
            "
        : "=r"(current_task->schd_ebp), "=r"(current_task->schd_esp)
        :
        : "ebp", "esp");
//        printf("before switch: %d\n", get_current_task()->pid);

        /* Restore previous esp and ebp state */
//        printf("after switch: %d\n", get_current_task()->pid);

        kprintf("Begin exchange stack[1/2], next_esp: 0x%x, next_ebp: 0x%x\n", next_task->schd_esp, next_task->schd_ebp);

        /* Then jump to execute return */
        asm volatile ("                  \n\
            movl %0, %%esp        \n\
            movl %1, %%ebp\n\
            "
        :
        : "r"(next_task->schd_esp), "r"(next_task->schd_ebp)
        : "memory", "cc", "ebp", "esp"
        );

//        kprintf("sched: next: %d esp: 0x%x, ebp: 0x%x\n", next_task->pid, next_task->schd_esp, next_task->schd_ebp);
//        kprintf("sched\n");
        return;
    }

    /* printf("Nothing to schedule...\n"); */
}
