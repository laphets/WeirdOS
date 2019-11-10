#ifndef _TASK_H
#define _TASK_H

#include "lib.h"
#include "vfs.h"

#define MAX_ARGUMENT_SIZE_EACH 20
#define MAX_ARGUMENT_SZIE 2048
#define MAX_ARGUMENT_NUM 20
#define MAX_FD_NUM 8
#define IMAGE_START_ADDR 0x08048000
#define FILE_NAME_SIZE 32

/* max task we can have */
#define MAX_TASK_NUM 8

#define KERNEL_BOTTOM 0x800000
#define PCB_BLOCK_SIZE 0x2000

#define TASK_STACK_SATRT_ADDR 0x8400000

/* Record for current running processes */
uint32_t current_task_num;

/* Struct define for PCB */
typedef struct task {
    uint8_t present;
    uint32_t pid;
    int32_t parent;
    void* parent_addr;
    uint32_t fd_size;
    file_desc_t fd_table[MAX_FD_NUM];
    char name[FILE_NAME_SIZE + 1];

    char full_argument[MAX_ARGUMENT_SZIE];

    uint32_t argument_num;
    char argument_list[MAX_ARGUMENT_NUM][MAX_ARGUMENT_SIZE_EACH];

    uint32_t prev_esp;
    uint32_t prev_ebp;
} task_t;

/* Init for all the task structures */
void init_tasks();

/* Get current task by the esp position */
task_t* get_current_task();

/* Copy data from kernel to user space */
void* copy_to_user(int8_t* dest, const int8_t* src, uint32_t n);
/* Copy data from user space to kernel */
void* copy_from_user(int8_t* dest, const int8_t* src, uint32_t n);

#endif //_42_TASK_H
