#ifndef _TASK_H
#define _TASK_H

#include "lib.h"
#include "vfs.h"
#include "terminal.h"

#define MAX_ARGUMENT_SIZE_EACH 20
#define MAX_ARGUMENT_SZIE 2048
#define MAX_ARGUMENT_NUM 20
#define MAX_FD_NUM 8
#define IMAGE_START_ADDR 0x08048000
#define FILE_NAME_SIZE 32

/* max task we can have */
#define MAX_TASK_NUM 16

#define KERNEL_BOTTOM 0x800000
#define PCB_BLOCK_SIZE 0x2000

#define TASK_STACK_SATRT_ADDR 0x8400000

/* Record for current running processes */
uint32_t current_task_num;

volatile uint8_t execute_as_root;

/* Struct define for PCB */
typedef struct task {
    uint8_t present;
    int32_t pid;
    int32_t parent;
    struct task * parent_task;
    uint32_t fd_size;
    file_desc_t fd_table[MAX_FD_NUM];
    char name[FILE_NAME_SIZE + 1];

    char full_argument[MAX_ARGUMENT_SZIE];

    uint32_t argument_num;
    char argument_list[MAX_ARGUMENT_NUM][MAX_ARGUMENT_SIZE_EACH];

    uint32_t prev_esp;
    uint32_t prev_ebp;

    uint32_t schd_esp;
    uint32_t schd_ebp;

    uint8_t* video_addr; /* For videomap */

    uint32_t terminal_id;

    volatile char rtc_wait;
    uint32_t rtc_hertz;
} task_t;

/**
 * Init for all the task structures
 * This fuction is going to initialize all the PCBs into kernel stack
 * The first task is starts at [8MB-8KB, 8MB]
 * The general task is at [8MB-(pid+1)*8KB, 8MB-(pid)*8KB]
 */
void init_tasks();

/**
 * Reset task to init state
 * @param task pointer to task
 */
void reset_task(task_t* task);

/**
 * Set task for some pid
 * @param task task to set
 */
void set_task(task_t* task);

/* Get current task by the esp position */
task_t* get_current_task();

/**
 * Get task by the provided pid
 * @param pid the pid for the task
 * @return task_t struct
 */
task_t* get_task_by_pid(uint32_t pid);

/**
 * Get a available pid for next task
 * @return pid or -1 if it's full
 */
int32_t get_avil_pid();

/* Copy data from kernel to user space */
void* copy_to_user(int8_t* dest, const int8_t* src, uint32_t n);
/* Copy data from user space to kernel */
void* copy_from_user(int8_t* dest, const int8_t* src, uint32_t n);

#endif //_42_TASK_H
